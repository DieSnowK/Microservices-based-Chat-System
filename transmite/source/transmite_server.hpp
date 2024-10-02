#include <brpc/server.h>
#include <butil/logging.h>
#include "etcd.hpp"
#include "logger.hpp"
#include "rabbitmq.hpp"
#include "channel.hpp"
#include "utils.hpp"
#include "mysql_chat_session_member.hpp"
#include "base.pb.h"
#include "user.pb.h"
#include "transmite.pb.h"

namespace SnowK
{
    class TransmiteServiceImpl : public SnowK::MsgTransmitService
    {
    public:
        TransmiteServiceImpl(const std::string &user_service_name,
                             const ServiceManager::ptr &channels,
                             const std::shared_ptr<odb::core::database> &mysql_client,
                             const std::string &exchange_name,
                             const std::string &routing_key,
                             const MQClient::ptr &mq_client) 
                             : _user_service_name(user_service_name)
                             , _svrmgr_channels(channels)
                             , _mysql_session_member_table(std::make_shared<ChatSessionMemeberTable>(mysql_client))
                             , _exchange_name(exchange_name)
                             , _routing_key(routing_key)
                             , _mq_client(mq_client) 
        {}
        ~TransmiteServiceImpl() {}

        void GetTransmitTarget(google::protobuf::RpcController *controller,
                               const ::SnowK::NewMessageReq *request,
                               ::SnowK::GetTransmitTargetRsp *response,
                               ::google::protobuf::Closure *done) override
        {
            brpc::ClosureGuard rpc_guard(done);
            auto Err_Response = [this, response](const std::string &rid,
                                                 const std::string &errmsg) -> void
            {
                response->set_request_id(rid);
                response->set_success(false);
                response->set_errmsg(errmsg);
                return;
            };

            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string chat_ssid = request->chat_session_id();
            const MessageContent &content = request->message();

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                LOG_ERROR("{}-{} There are no user subservice nodes available for access", rid, _user_service_name);
                return Err_Response(rid, "There are no user subservice nodes available for access");
            }

            // Get sender information
            UserService_Stub stub(channel.get());
            brpc::Controller cntl;
            GetUserInfoReq req;
            GetUserInfoRsp rsp;
            req.set_request_id(rid);
            req.set_user_id(uid);

            stub.GetUserInfo(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed() == true || rsp.success() == false)
            {
                LOG_ERROR("{} - User subservice call failed: {}", request->request_id(), cntl.ErrorText());
                return Err_Response(request->request_id(), "User subservice call failed");
            }

            // Organize information
            MessageInfo message;
            message.set_message_id(UUID());
            message.set_chat_session_id(chat_ssid);
            message.set_timestamp(time(nullptr));
            message.mutable_sender()->CopyFrom(rsp.user_info());
            message.mutable_message()->CopyFrom(content);

            // The encapsulated message is published to the 
            // message queue and the message storage subservice is persisted
            if (_mq_client->Publish(_exchange_name, message.SerializeAsString(), _routing_key) == false)
            {
                LOG_ERROR("{} - Failed to publish a persistent message: {}", request->request_id(), cntl.ErrorText());
                return Err_Response(request->request_id(), "Failed to publish a persistent message");
            }

            // Obtain the list of users of the message forwarding client
            auto target_list = _mysql_session_member_table->Members(chat_ssid);

            // Organize response
            response->set_request_id(rid);
            response->set_success(true);
            response->mutable_message()->CopyFrom(message);
            for (const auto &id : target_list)
            {
                response->add_target_id_list(id);
            }
        }

    private:
        std::string _user_service_name;
        ServiceManager::ptr _svrmgr_channels;

        ChatSessionMemeberTable::ptr _mysql_session_member_table;

        std::string _exchange_name;
        std::string _routing_key;
        MQClient::ptr _mq_client;
    };

    class TransmiteServer
    {
    public:
        using ptr = std::shared_ptr<TransmiteServer>;
        TransmiteServer(const std::shared_ptr<odb::core::database> &mysql_client,
                        const Discovery::ptr discovery_client,
                        const Registry::ptr &registry_client,
                        const std::shared_ptr<brpc::Server> &server) 
                : _service_discoverer(discovery_client)
                , _registry_client(registry_client)
                , _mysql_client(mysql_client)
                , _rpc_server(server) 
        {}
        ~TransmiteServer() {}

        void Start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }

    private:
        Discovery::ptr _service_discoverer;
        Registry::ptr _registry_client;
        std::shared_ptr<odb::core::database> _mysql_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };

    // Builder Pattern
    class TransmiteServerBuilder
    {
    public:
        void Make_MySQL_Object(const std::string &user,
                               const std::string &pwd,
                               const std::string &host,
                               const std::string &db,
                               const std::string &cset,
                               int port,
                               int conn_pool_count)
        {
            _mysql_client = ODBFactory::Create(user, pwd, host, db, cset, port, conn_pool_count);
        }

        // TODO
        void Make_Discovery_Object(const std::string &reg_host,
                                   const std::string &base_service_name,
                                   const std::string &user_service_name)
        {
            _user_service_name = user_service_name;
            _svrmgr_channels = std::make_shared<ServiceManager>();
            _svrmgr_channels->Declare(user_service_name);

            LOG_DEBUG("Set the user sub-service as the sub-service to be added and managed: {}", user_service_name);

            auto put_cb = std::bind(&ServiceManager::ServiceOnline, _svrmgr_channels.get(), std::placeholders::_1, std::placeholders::_2);
            auto del_cb = std::bind(&ServiceManager::ServiceOffline, _svrmgr_channels.get(), std::placeholders::_1, std::placeholders::_2);
            _service_discoverer = std::make_shared<Discovery>(reg_host, base_service_name, put_cb, del_cb);
        }

        void Make_Registry_Object(const std::string &reg_host,
                                  const std::string &service_name,
                                  const std::string &access_host)
        {
            _registry_client = std::make_shared<Registry>(reg_host);
            _registry_client->Registry_Service(service_name, access_host);
        }

        // 用于构造rabbitmq客户端对象
        void Make_Mq_Object(const std::string &user,
                            const std::string &pwd,
                            const std::string &host,
                            const std::string &exchange_name,
                            const std::string &queue_name,
                            const std::string &binding_key)
        {
            _routing_key = binding_key;
            _exchange_name = exchange_name;
            _mq_client = std::make_shared<MQClient>(user, pwd, host);
            _mq_client->DeclareComponents(exchange_name, queue_name, binding_key);
        }

        void Make_Rpc_server(uint16_t port, int32_t timeout, uint8_t num_threads)
        {
            if (!_mq_client)
            {
                LOG_ERROR("The Message Queuing client module has not been initialized!");
                abort();
            }
            if (!_svrmgr_channels)
            {
                LOG_ERROR("The channel management module has not been initialized");
                abort();
            }
            if (!_mysql_client)
            {
                LOG_ERROR("The Mysql database module has not been initialized");
                abort();
            }

            _rpc_server = std::make_shared<brpc::Server>();

            TransmiteServiceImpl *transmite_service = new TransmiteServiceImpl(
                _user_service_name, _svrmgr_channels, _mysql_client, _exchange_name, _routing_key, _mq_client);

            if (_rpc_server->AddService(transmite_service,
                                        brpc::ServiceOwnership::SERVER_OWNS_SERVICE) == -1)
            {
                LOG_ERROR("Failed to add RPC service");
                abort();
            }

            brpc::ServerOptions options;
            options.idle_timeout_sec = timeout;
            options.num_threads = num_threads;
            if (_rpc_server->Start(port, &options) == -1)
            {
                LOG_ERROR("Service startup failed");
                abort();
            }
        }

        TransmiteServer::ptr Build()
        {
            if (!_service_discoverer)
            {
                LOG_ERROR("The service discovery module has not been initialized");
                abort();
            }
            if (!_registry_client)
            {
                LOG_ERROR("The service registration module has not been initialized");
                abort();
            }
            if (!_rpc_server)
            {
                LOG_ERROR("The rpc server has not been initialized");
                abort();
            }

            TransmiteServer::ptr server = std::make_shared<TransmiteServer>(
                _mysql_client, _service_discoverer, _registry_client, _rpc_server);

            return server;
        }

    private:
        std::string _user_service_name;
        ServiceManager::ptr _svrmgr_channels;
        Discovery::ptr _service_discoverer;

        std::string _routing_key;
        std::string _exchange_name;
        MQClient::ptr _mq_client;

        Registry::ptr _registry_client;
        std::shared_ptr<odb::core::database> _mysql_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };
}