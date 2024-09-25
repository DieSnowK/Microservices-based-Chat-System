#include <brpc/server.h>
#include <butil/logging.h>
#include "asr.hpp"
#include "etcd.hpp"
#include "logger.hpp"
#include "speech.pb.h"

namespace SnowK
{
    class SpeechServiceImpl : public SnowK::SpeechService
    {
    public:
        SpeechServiceImpl(const ASRClient::ptr &asr_client)
            : _asr_client(asr_client)
        {}
        ~SpeechServiceImpl()
        {}

        void SpeechRecognition(google::protobuf::RpcController *controller,
                               const ::SnowK::SpeechRecognitionReq *request,
                               ::SnowK::SpeechRecognitionRsp *response,
                               ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("A speech-to-text request is received");
            brpc::ClosureGuard rpc_guard(done);

            std::string err;
            std::string ret = _asr_client->Recognize(request->speech_content(), err);
            if (ret.empty())
            {
                LOG_ERROR("{} speech recognition failed", request->request_id());

                response->set_request_id(request->request_id());
                response->set_success(false);
                response->set_errmsg("speech recognition failed:" + err);

                return;
            }

            response->set_request_id(request->request_id());
            response->set_success(true);
            response->set_recognition_result(ret);
        }

    private:
        ASRClient::ptr _asr_client;
    };

    // TODO 考虑是否维护这么多成员变量
    class SpeechServer
    {
    public:
        using ptr = std::shared_ptr<SpeechServer>;

        SpeechServer(const ASRClient::ptr &asr_client,
                     const Registry::ptr &reg_client,
                     const std::shared_ptr<brpc::Server> &server)
            : _asr_client(asr_client)
            , _reg_client(reg_client)
            , _rpc_server(server) 
        {}
        
        ~SpeechServer() {}

        void Start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }

    private:
        ASRClient::ptr _asr_client;
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };

    // Builder Pattern
    // SpeechServer构造依赖一定次序
    class SpeechServerBuilder
    {
    public:
        void Make_Asr_Object(const std::string &app_id,
                             const std::string &api_key,
                             const std::string &secret_key)
        {
            _asr_client = std::make_shared<ASRClient>(app_id, api_key, secret_key);
        }

        void Make_Reg_Object(const std::string &reg_host,
                             const std::string &service_name,
                             const std::string &access_host)
        {
            _reg_client = std::make_shared<Registry>(reg_host);
            _reg_client->Registry_Service(service_name, access_host);
        }

        void Make_RPC_Server(uint16_t port, int32_t timeout, uint8_t num_threads)
        {
            if (!_asr_client)
            {
                LOG_ERROR("The speech recognition module has not been initialized");
                abort();
            }

            _rpc_server = std::make_shared<brpc::Server>();
            SpeechServiceImpl *speech_service = new SpeechServiceImpl(_asr_client);
            if (_rpc_server->AddService(speech_service,
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
                LOG_ERROR("The service failed to start");
                abort();
            }
        }

        SpeechServer::ptr Build()
        {
            if (!_asr_client)
            {
                LOG_ERROR("The speech recognition module has not been initialized");
                abort();
            }
            if (!_reg_client)
            {
                LOG_ERROR("The service registration module has not been initialized");
                abort();
            }
            if (!_rpc_server)
            {
                LOG_ERROR("The rpc server has not been initialized");
                abort();
            }

            SpeechServer::ptr server = std::make_shared<SpeechServer>(
                _asr_client, _reg_client, _rpc_server);

            return server;
        }

    private:
        ASRClient::ptr _asr_client;
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };
}