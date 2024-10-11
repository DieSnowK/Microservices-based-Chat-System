#include <brpc/server.h>
#include <butil/logging.h>

#include "es.hpp"
#include "data_es.hpp"
#include "mysql_message.hpp"
#include "etcd.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "channel.hpp"
#include "rabbitmq.hpp"

#include "base.pb.h"
#include "file.pb.h"
#include "user.pb.h"
#include "message.pb.h"

namespace SnowK
{
    class MessageServiceImpl : public SnowK::MsgStorageService
    {
    public:
        MessageServiceImpl(const std::shared_ptr<elasticlient::Client> &es_client,
                           const std::shared_ptr<odb::core::database> &mysql_client,
                           const ServiceManager::ptr &channel_manager,
                           const std::string &file_service_name,
                           const std::string &user_service_name)
            : _es_message(std::make_shared<ESMessage>(es_client))
            , _mysql_message(std::make_shared<MessageTable>(mysql_client))
            , _file_service_name(file_service_name)
            , _user_service_name(user_service_name)
            , _svrmgr_channels(channel_manager)
        {
            _es_message->CreateIndex();
        }

        ~MessageServiceImpl() {}

        virtual void GetHistoryMsg(::google::protobuf::RpcController *controller,
                                   const ::SnowK::GetHistoryMsgReq *request,
                                   ::SnowK::GetHistoryMsgRsp *response,
                                   ::google::protobuf::Closure *done)
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
            std::string chat_ssid = request->chat_session_id();
            boost::posix_time::ptime stime = boost::posix_time::from_time_t(request->start_time());
            boost::posix_time::ptime etime = boost::posix_time::from_time_t(request->over_time());

            auto msg_lists = _mysql_message->Range(chat_ssid, stime, etime);
            if (msg_lists.empty())
            {
                response->set_request_id(rid);
                response->set_success(true);
                return;
            }

            // Collects file IDs for all file type messages
            std::unordered_set<std::string> file_id_lists;
            for (const auto &msg : msg_lists)
            {
                if (msg.File_Id().empty()) // Non-text messages
                {
                    continue;
                }

                LOG_DEBUG("The ID of the file that needs to be downloaded: {}", msg.File_Id());
                file_id_lists.insert(msg.File_Id());
            }

            // Bulk file downloads from the Files subservice
            std::unordered_map<std::string, std::string> file_data_lists;
            if (_GetFile(rid, file_id_lists, file_data_lists) == false)
            {
                LOG_ERROR("{} - Failed to download batch file data", rid);
                return Err_Response(rid, "Failed to download batch file data");
            }

            // Collects the sender user ID of all messages
            std::unordered_set<std::string> user_id_lists;
            for (const auto &msg : msg_lists)
            {
                user_id_lists.insert(msg.User_Id());
            }

            // Obtain bulk user information from user subservices
            std::unordered_map<std::string, UserInfo> user_lists;
            if (_GetUser(rid, user_id_lists, user_lists) == false)
            {
                LOG_ERROR("{} - Failed to obtain bulk user data", rid);
                return Err_Response(rid, "Failed to obtain bulk user data");
            }

            // Organize responses
            response->set_request_id(rid);
            response->set_success(true);
            for (const auto &msg : msg_lists)
            {
                auto message_info = response->add_msg_list();
                message_info->set_message_id(msg.Message_Id());
                message_info->set_chat_session_id(msg.Session_Id());
                message_info->set_timestamp(boost::posix_time::to_time_t(msg.Create_Time()));
                message_info->mutable_sender()->CopyFrom(user_lists[msg.User_Id()]);

                switch (msg.Message_Type())
                {
                case MessageType::STRING:
                    message_info->mutable_message()->set_message_type(MessageType::STRING);
                    message_info->mutable_message()->mutable_string_message()->set_content(msg.Content());
                    break;
                case MessageType::IMAGE:
                    message_info->mutable_message()->set_message_type(MessageType::IMAGE);
                    message_info->mutable_message()->mutable_image_message()->set_file_id(msg.File_Id());
                    message_info->mutable_message()->mutable_image_message()->set_image_content(file_data_lists[msg.File_Id()]);
                    break;
                case MessageType::FILE:
                    message_info->mutable_message()->set_message_type(MessageType::FILE);
                    message_info->mutable_message()->mutable_file_message()->set_file_id(msg.File_Id());
                    message_info->mutable_message()->mutable_file_message()->set_file_size(msg.File_Size());
                    message_info->mutable_message()->mutable_file_message()->set_file_name(msg.File_Name());
                    message_info->mutable_message()->mutable_file_message()->set_file_contents(file_data_lists[msg.File_Id()]);
                    break;
                case MessageType::SPEECH:
                    message_info->mutable_message()->set_message_type(MessageType::SPEECH);
                    message_info->mutable_message()->mutable_speech_message()->set_file_id(msg.File_Id());
                    message_info->mutable_message()->mutable_speech_message()->set_file_contents(file_data_lists[msg.File_Id()]);
                    break;
                default:
                    LOG_ERROR("The message type is incorrect");
                    return;
                }
            }

            return;
        } // end of GetHistoryMsg()

        virtual void GetRecentMsg(::google::protobuf::RpcController *controller,
                                  const ::SnowK::GetRecentMsgReq *request,
                                  ::SnowK::GetRecentMsgRsp *response,
                                  ::google::protobuf::Closure *done)
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
            std::string chat_ssid = request->chat_session_id();
            int msg_count = request->msg_count();

            auto msg_lists = _mysql_message->Recent(chat_ssid, msg_count);
            if (msg_lists.empty())
            {
                response->set_request_id(rid);
                response->set_success(true);
                return;
            }

            // Collects file IDs for all file type messages
            std::unordered_set<std::string> file_id_lists;
            for (const auto &msg : msg_lists)
            {
                if (msg.File_Id().empty()) // Non-text messages
                {
                    continue;
                }

                LOG_DEBUG("The ID of the file that needs to be downloaded: {}", msg.File_Id());
                file_id_lists.insert(msg.File_Id());
            }

            // Bulk file downloads from the Files subservice
            std::unordered_map<std::string, std::string> file_data_lists;
            if (_GetFile(rid, file_id_lists, file_data_lists) == false)
            {
                LOG_ERROR("{} - Failed to download batch file data", rid);
                return Err_Response(rid, "Failed to download batch file data");
            }

            // Collects the sender user ID of all messages
            std::unordered_set<std::string> user_id_lists;
            for (const auto &msg : msg_lists)
            {
                user_id_lists.insert(msg.User_Id());
            }

            // Obtain bulk user information from user subservices
            std::unordered_map<std::string, UserInfo> user_lists;
            if (_GetUser(rid, user_id_lists, user_lists) == false)
            {
                LOG_ERROR("{} - Failed to obtain bulk user data", rid);
                return Err_Response(rid, "Failed to obtain bulk user data");
            }

            // Organize responses
            response->set_request_id(rid);
            response->set_success(true);
            for (const auto &msg : msg_lists)
            {
                auto message_info = response->add_msg_list();
                message_info->set_message_id(msg.Message_Id());
                message_info->set_chat_session_id(msg.Session_Id());
                message_info->set_timestamp(boost::posix_time::to_time_t(msg.Create_Time()));
                message_info->mutable_sender()->CopyFrom(user_lists[msg.User_Id()]);

                switch (msg.Message_Type())
                {
                case MessageType::STRING:
                    message_info->mutable_message()->set_message_type(MessageType::STRING);
                    message_info->mutable_message()->mutable_string_message()->set_content(msg.Content());
                    break;
                case MessageType::IMAGE:
                    message_info->mutable_message()->set_message_type(MessageType::IMAGE);
                    message_info->mutable_message()->mutable_image_message()->set_file_id(msg.File_Id());
                    message_info->mutable_message()->mutable_image_message()->set_image_content(file_data_lists[msg.File_Id()]);
                    break;
                case MessageType::FILE:
                    message_info->mutable_message()->set_message_type(MessageType::FILE);
                    message_info->mutable_message()->mutable_file_message()->set_file_id(msg.File_Id());
                    message_info->mutable_message()->mutable_file_message()->set_file_size(msg.File_Size());
                    message_info->mutable_message()->mutable_file_message()->set_file_name(msg.File_Name());
                    message_info->mutable_message()->mutable_file_message()->set_file_contents(file_data_lists[msg.File_Id()]);
                    break;
                case MessageType::SPEECH:
                    message_info->mutable_message()->set_message_type(MessageType::SPEECH);
                    message_info->mutable_message()->mutable_speech_message()->set_file_id(msg.File_Id());
                    message_info->mutable_message()->mutable_speech_message()->set_file_contents(file_data_lists[msg.File_Id()]);
                    break;
                default:
                    LOG_ERROR("The message type is incorrect");
                    return;
                }
            }

            return;
        } // end of GetRecentMsg

        // Message search for keywords – for text messages only
        virtual void MsgSearch(::google::protobuf::RpcController *controller,
                               const ::SnowK::MsgSearchReq *request,
                               ::SnowK::MsgSearchRsp *response,
                               ::google::protobuf::Closure *done)
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
            std::string chat_ssid = request->chat_session_id();
            std::string skey = request->search_key();

            auto msg_lists = _es_message->Search(skey, chat_ssid);
            if (msg_lists.empty())
            {
                response->set_request_id(rid);
                response->set_success(true);
                return;
            }

            // Collects the sender user ID of all messages
            std::unordered_set<std::string> user_id_lists;
            for (const auto &msg : msg_lists)
            {
                user_id_lists.insert(msg.User_Id());
            }

            // Obtain bulk user information from user subservices
            std::unordered_map<std::string, UserInfo> user_lists;
            if (_GetUser(rid, user_id_lists, user_lists) == false)
            {
                LOG_ERROR("{} - Failed to obtain bulk user data", rid);
                return Err_Response(rid, "Failed to obtain bulk user data");
            }

            // Organize responses
            response->set_request_id(rid);
            response->set_success(true);
            for (const auto &msg : msg_lists)
            {
                auto message_info = response->add_msg_list();
                message_info->set_message_id(msg.Message_Id());
                message_info->set_chat_session_id(msg.Session_Id());
                message_info->set_timestamp(boost::posix_time::to_time_t(msg.Create_Time()));
                message_info->mutable_sender()->CopyFrom(user_lists[msg.User_Id()]);
                message_info->mutable_message()->set_message_type(MessageType::STRING);
                message_info->mutable_message()->mutable_string_message()->set_content(msg.Content());
            }

            return;
        }

        void OnMessage(const char *body, size_t sz)
        {
            SnowK::MessageInfo message;
            if (message.ParseFromArray(body, sz) == false)
            {
                LOG_ERROR("Failed to deserialize the consumed message");
                return;
            }

            // Different processing is carried out according to different message types
            std::string file_id, file_name, content;
            int64_t file_size;
            switch (message.message().message_type())
            {
            // If it is a text message, the meta information is stored in Elasticsearch data
            case MessageType::STRING:
                content = message.message().string_message().content();
                if (_es_message->AppendData(message.sender().user_id(), message.message_id(),
                                            message.timestamp(), message.chat_session_id(), content) == false)
                {
                    LOG_ERROR("Failed to store text messages to the storage engine");
                    return;
                }
                break;
            // If it is a picture/voice/file message, 
            // the data is retrieved and stored in the file subservice and the file ID is obtained
            case MessageType::IMAGE:
            {
                const auto &msg = message.message().image_message();
                if (_PutFile("", msg.image_content(), msg.image_content().size(), file_id) == false)
                {
                    LOG_ERROR("Failed to upload image to file subservice");
                    return;
                }
                break;
            }
            case MessageType::FILE:
            {
                const auto &msg = message.message().file_message();
                file_name = msg.file_name();
                file_size = msg.file_size();
                if (_PutFile(file_name, msg.file_contents(), file_size, file_id) == false)
                {
                    LOG_ERROR("Failed to upload files to the file subservice");
                    return;
                }
                break;
            }
            case MessageType::SPEECH:
            {
                const auto &msg = message.message().speech_message();
                if (_PutFile("", msg.file_contents(), msg.file_contents().size(), file_id) == false)
                {
                    LOG_ERROR("Failed to upload voice to file subservice");
                    return;
                }
                break;
            }
            default:
                LOG_ERROR("Message type incorrect");
                return;
            } // end of switch (message.message().message_type())

            // Extract the meta information of the message and store it in the MySQL database
            SnowK::Message msg(message.message_id(), message.chat_session_id(),
                               message.sender().user_id(), message.message().message_type(),
                               boost::posix_time::from_time_t(message.timestamp()));
            msg.Content(content);
            msg.File_Id(file_id);
            msg.File_Name(file_name);
            msg.File_Size(file_size);

            if (_mysql_message->Insert(msg) == false)
            {
                LOG_ERROR("Failed to insert a new message into the database");
                return;
            }
        } // end of OnMessage

    private:
        bool _GetUser(const std::string &rid,
                      const std::unordered_set<std::string> &user_id_lists,
                      std::unordered_map<std::string, UserInfo> &user_lists)
        {
            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                LOG_ERROR("{} There are no user sub-service nodes available for access", 
                          _user_service_name);
                return false;
            }

            UserService_Stub stub(channel.get());

            brpc::Controller cntl;
            GetMultiUserInfoReq req;
            GetMultiUserInfoRsp rsp;
            req.set_request_id(rid);
            for (const auto &id : user_id_lists)
            {
                req.add_users_id(id);
            }

            stub.GetMultiUserInfo(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed() == true || rsp.success() == false)
            {
                LOG_ERROR("The user subservice call failed: {}", cntl.ErrorText());
                return false;
            }

            const auto &umap = rsp.users_info();
            for (auto it = umap.begin(); it != umap.end(); ++it)
            {
                user_lists.insert(std::make_pair(it->first, it->second));
            }

            return true;
        }

        // TODO why unordered_map?
        bool _GetFile(const std::string &rid,
                      const std::unordered_set<std::string> &file_id_lists,
                      std::unordered_map<std::string, std::string> &file_data_lists)
        {
            auto channel = _svrmgr_channels->Choose(_file_service_name);
            if (!channel)
            {
                LOG_ERROR("{} There are no file sub-service nodes available for access", 
                          _file_service_name);
                return false;
            }

            FileService_Stub stub(channel.get());

            brpc::Controller cntl;
            GetMultiFileReq req;
            GetMultiFileRsp rsp;
            req.set_request_id(rid);
            for (const auto &id : file_id_lists)
            {
                req.add_file_id_list(id);
            }

            stub.GetMultiFile(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed() == true || rsp.success() == false)
            {
                LOG_ERROR("The file subservice call failed: {}", cntl.ErrorText());
                return false;
            }

            const auto &fmap = rsp.file_data();
            for (auto it = fmap.begin(); it != fmap.end(); ++it)
            {
                file_data_lists.insert(std::make_pair(it->first, it->second.file_content()));
            }

            return true;
        }

        bool _PutFile(const std::string &filename, const std::string &body,
                      const int64_t fsize, std::string &file_id)
        {
            auto channel = _svrmgr_channels->Choose(_file_service_name);
            if (!channel)
            {
                LOG_ERROR("{} There are no file sub-service nodes available for access",
                          _file_service_name);
                return false;
            }

            FileService_Stub stub(channel.get());

            brpc::Controller cntl;
            PutSingleFileReq req;
            PutSingleFileRsp rsp;
            req.mutable_file_data()->set_file_name(filename);
            req.mutable_file_data()->set_file_size(fsize);
            req.mutable_file_data()->set_file_content(body);

            stub.PutSingleFile(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed() == true || rsp.success() == false)
            {
                LOG_ERROR("The file subservice call failed: {}", cntl.ErrorText());
                return false;
            }
            file_id = rsp.file_info().file_id();

            return true;
        }

    private:
        ESMessage::ptr _es_message;
        MessageTable::ptr _mysql_message;
        
        std::string _user_service_name;
        std::string _file_service_name;
        ServiceManager::ptr _svrmgr_channels;
    };

    class MessageServer
    {
    public:
        using ptr = std::shared_ptr<MessageServer>;
        
        MessageServer(const MQClient::ptr &mq_client,
                      const Discovery::ptr service_discoverer,
                      const Registry::ptr &reg_client,
                      const std::shared_ptr<elasticlient::Client> &es_client,
                      const std::shared_ptr<odb::core::database> &mysql_client,
                      const std::shared_ptr<brpc::Server> &server) 
            : _mq_client(mq_client)
            , _service_discoverer(service_discoverer)
            , _registry_client(reg_client)
            , _es_client(es_client)
            , _mysql_client(mysql_client)
            , _rpc_server(server) 
        {}
        ~MessageServer() {}

        void Start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }

    private:
        Discovery::ptr _service_discoverer;
        Registry::ptr _registry_client;
        MQClient::ptr _mq_client;
        std::shared_ptr<elasticlient::Client> _es_client;
        std::shared_ptr<odb::core::database> _mysql_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    }; // end of MessageServer

    // Builder Pattern
    class MessageServerBuilder
    {
    public:
        void Make_Es_Object(const std::vector<std::string> host_list)
        {
            _es_client = ESClientFactory::Create(host_list);
        }

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
                                   const std::string &file_service_name,
                                   const std::string &user_service_name)
        {
            _user_service_name = user_service_name;
            _file_service_name = file_service_name;
            _svrmgr_channels = std::make_shared<ServiceManager>();
            _svrmgr_channels->Declare(file_service_name);
            _svrmgr_channels->Declare(user_service_name);

            LOG_DEBUG("Set the file sub-service as the sub-service to be added and managed: {}", file_service_name);
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

        void Make_Mq_Object(const std::string &user,
                            const std::string &pwd,
                            const std::string &host,
                            const std::string &exchange_name,
                            const std::string &queue_name,
                            const std::string &binding_key)
        {
            _exchange_name = exchange_name;
            _queue_name = queue_name;
            _mq_client = std::make_shared<MQClient>(user, pwd, host);
            _mq_client->DeclareComponents(exchange_name, queue_name, binding_key);
        }

        void Make_Rpc_Server(uint16_t port, int32_t timeout, uint8_t num_threads)
        {
            if (!_es_client)
            {
                LOG_ERROR("The ES search engine module has not been initialized");
                abort();
            }
            if (!_mysql_client)
            {
                LOG_ERROR("The Mysql database module has not been initialized");
                abort();
            }
            if (!_svrmgr_channels)
            {
                LOG_ERROR("The channel management module has not been initialized");
                abort();
            }

            _rpc_server = std::make_shared<brpc::Server>();

            MessageServiceImpl *msg_service = new MessageServiceImpl(_es_client, _mysql_client, _svrmgr_channels, 
                                                                     _file_service_name, _user_service_name);
            if (_rpc_server->AddService(msg_service, brpc::ServiceOwnership::SERVER_OWNS_SERVICE) == -1)
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

            auto callback = std::bind(&MessageServiceImpl::OnMessage, msg_service,
                                      std::placeholders::_1, std::placeholders::_2);
            _mq_client->Consume(_queue_name, callback);
        }

        MessageServer::ptr Build()
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

            MessageServer::ptr server = std::make_shared<MessageServer>(
                _mq_client, _service_discoverer, _registry_client,
                _es_client, _mysql_client, _rpc_server);

            return server;
        }

    private:
        Registry::ptr _registry_client;

        std::shared_ptr<elasticlient::Client> _es_client;
        std::shared_ptr<odb::core::database> _mysql_client;

        std::string _user_service_name;
        std::string _file_service_name;
        ServiceManager::ptr _svrmgr_channels;
        Discovery::ptr _service_discoverer;

        std::string _exchange_name;
        std::string _queue_name;
        MQClient::ptr _mq_client;

        std::shared_ptr<brpc::Server> _rpc_server;
    }; // end of MessageServerBuilder
}
