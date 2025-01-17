#include <brpc/server.h>
#include <butil/logging.h>
#include "etcd.hpp"   
#include "logger.hpp"
#include "utils.hpp"
#include "base.pb.h"
#include "file.pb.h"

namespace SnowK
{
    // RPC -> Remote Interface
    class FileServiceImpl : public SnowK::FileService
    {
    private:
        template <class T>
        void Err_Response(T *response, const std::string &rid, const std::string &errmsg)
        {
            response->set_request_id(rid);
            response->set_success(false);
            response->set_errmsg(errmsg);

            LOG_ERROR("{} - {}", rid, errmsg);
        }

    public:
        FileServiceImpl(const std::string &storage_path) 
            : _storage_path(storage_path)
        {
            umask(0);
            mkdir(storage_path.c_str(), 0775);
            if (_storage_path.back() != '/')
            {
                _storage_path.push_back('/');
            }
        }

        ~FileServiceImpl() {}

        virtual void GetSingleFile(google::protobuf::RpcController *controller,
                           const ::SnowK::GetSingleFileReq *request,
                           ::SnowK::GetSingleFileRsp *response,
                           ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);
            std::string rid = request->request_id();

            std::string fid = request->file_id();
            std::string filename = _storage_path + fid;
            std::string body;
            if (ReadFile(filename, body) == false)
            {
                return Err_Response<::SnowK::GetSingleFileRsp>(response, rid,
                        "Failed to read file data");
            }

            response->set_request_id(rid);
            response->set_success(true);
            response->mutable_file_data()->set_file_id(fid);
            response->mutable_file_data()->set_file_content(body);
        }

        virtual void GetMultiFile(google::protobuf::RpcController *controller,
                                    const ::SnowK::GetMultiFileReq *request,
                                    ::SnowK::GetMultiFileRsp *response,
                                    ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);
            std::string rid = request->request_id();

            for (int i = 0; i < request->file_id_list_size(); i++)
            {
                std::string fid = request->file_id_list(i);
                std::string filename = _storage_path + fid;
                std::string body;
                if (ReadFile(filename, body) == false)
                {
                    return Err_Response<::SnowK::GetMultiFileRsp>(response, rid,
                        "Failed to read file data");
                }

                FileDownloadData data;
                data.set_file_id(fid);
                data.set_file_content(body);
                response->mutable_file_data()->insert({fid, data});
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

        virtual void PutSingleFile(google::protobuf::RpcController *controller,
                                    const ::SnowK::PutSingleFileReq *request,
                                    ::SnowK::PutSingleFileRsp *response,
                                    ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);
            std::string rid = request->request_id();
            

            std::string fid = UUID();
            std::string filename = _storage_path + fid;
            if (WriteFile(filename, request->file_data().file_content()) == false)
            {
                return Err_Response<::SnowK::PutSingleFileRsp>(response, rid,
                        "Failed to write file data");
            }

            response->set_request_id(rid);
            response->set_success(true);
            response->mutable_file_info()->set_file_id(fid);
            response->mutable_file_info()->set_file_size(request->file_data().file_size());
            response->mutable_file_info()->set_file_name(request->file_data().file_name());
        }

        virtual void PutMultiFile(google::protobuf::RpcController *controller,
                                    const ::SnowK::PutMultiFileReq *request,
                                    ::SnowK::PutMultiFileRsp *response,
                                    ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);
            std::string rid = request->request_id();

            for (int i = 0; i < request->file_data_size(); i++)
            {
                std::string fid = UUID();
                std::string filename = _storage_path + fid;
                if (WriteFile(filename, request->file_data(i).file_content()) == false)
                {
                    return Err_Response<::SnowK::PutMultiFileRsp>(response, rid,
                            "Failed to write file data");
                }

                SnowK::FileMessageInfo *info = response->add_file_info();
                info->set_file_id(fid);
                info->set_file_size(request->file_data(i).file_size());
                info->set_file_name(request->file_data(i).file_name());
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

    private:
        std::string _storage_path;
    };

    class FileServer
    {
    public:
        using ptr = std::shared_ptr<FileServer>;
        
        FileServer(const Registry::ptr &reg_client,
                   const std::shared_ptr<brpc::Server> &server) 
            : _reg_client(reg_client)
            , _rpc_server(server) 
        {}

        ~FileServer() {}

        void Start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }

    private:
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };

    // Builder Pattern
    class FileServerBuilder
    {
    public:
        void Make_Reg_Object(const std::string &reg_host,
                             const std::string &service_name,
                             const std::string &access_host)
        {
            _reg_client = std::make_shared<Registry>(reg_host);
            _reg_client->Registry_Service(service_name, access_host);
        }

        void Make_Rpc_Server(uint16_t port, int32_t timeout,
                             uint8_t num_threads, const std::string &path = "./data/")
        {
            _rpc_server = std::make_shared<brpc::Server>();
            FileServiceImpl *file_service = new FileServiceImpl(path);
            if (_rpc_server->AddService(file_service,
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

        FileServer::ptr Build()
        {
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

            FileServer::ptr server = std::make_shared<FileServer>(_reg_client, _rpc_server);

            return server;
        }

    private:
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };
}