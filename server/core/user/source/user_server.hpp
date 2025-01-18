#pragma once
#include <cctype>
#include <brpc/server.h>
#include <butil/logging.h>

#include "es.hpp"
#include "mysql.hpp"
#include "redis.hpp"
#include "data_es.hpp"
#include "data_redis.hpp"
#include "mysql_user.hpp"
#include "etcd.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "dms.hpp"
#include "channel.hpp"

#include "user.hpp"
#include "user-odb.hxx"

#include "user.pb.h"
#include "base.pb.h"
#include "file.pb.h"

namespace SnowK
{
    class UserServiceImpl : public SnowK::UserService
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
        UserServiceImpl(const DMSClient::ptr &dms_client,
                        const std::shared_ptr<elasticlient::Client> &es_client,
                        const std::shared_ptr<odb::core::database> &mysql_client,
                        const std::shared_ptr<sw::redis::Redis> &redis_client,
                        const ServiceManager::ptr &channel_manager,
                        const std::string &file_service_name) 
            : _es_user(std::make_shared<ESUser>(es_client))
            , _mysql_user(std::make_shared<UserTable>(mysql_client))
            , _redis_session(std::make_shared<Session>(redis_client))
            , _redis_status(std::make_shared<Status>(redis_client))
            , _redis_codes(std::make_shared<Codes>(redis_client))
            , _file_service_name(file_service_name)
            , _svrmgr_channels(channel_manager)
            , _dms_client(dms_client)
        {
            _es_user->CreateIndex();
        }
        ~UserServiceImpl() {}

        virtual void UserRegister(::google::protobuf::RpcController *controller,
                                  const ::SnowK::UserRegisterReq *request,
                                  ::SnowK::UserRegisterRsp *response,
                                  ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("Received request");
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string nickname = request->nickname();
            std::string password = request->password();

            if (Nickname_Check(nickname) == false)
            {
                return Err_Response<::SnowK::UserRegisterRsp>(response, rid,
                        "The username length is invalid");
            }

            if (Password_Check(password) == false)
            {
                return Err_Response<::SnowK::UserRegisterRsp>(response, rid,
                        "The password format is invalid");
            }

            if (_mysql_user->Select_By_Nickname(nickname))
            {
                return Err_Response<::SnowK::UserRegisterRsp>(response, rid,
                        "The username is occupied");
            }

            std::string uid = UUID();
            auto user = std::make_shared<User>(uid, nickname, password);
            if (_mysql_user->Insert(user) == false)
            {
                return Err_Response<::SnowK::UserRegisterRsp>(response, rid,
                        "Failed to add data to MySQL database");
            }

            if (_es_user->AppendData(uid, "", nickname, "", "") == false)
            {
                return Err_Response<::SnowK::UserRegisterRsp>(response, rid,
                        "ES search engine failed to add data");
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

        virtual void UserLogin(::google::protobuf::RpcController *controller,
                               const ::SnowK::UserLoginReq *request,
                               ::SnowK::UserLoginRsp *response,
                               ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("Received request");
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string nickname = request->nickname();
            std::string password = request->password();
            
            auto user = _mysql_user->Select_By_Nickname(nickname);
            if (!user || password != user->Password())
            {
                return Err_Response<::SnowK::UserLoginRsp>(response, rid,
                        "Wrong username or password");
            }

            if (_redis_status->Exists(user->User_Id()))
            {
                return Err_Response<::SnowK::UserLoginRsp>(response, rid,
                        "The user is already logged in elsewhere");
            }

            // Construct session ID, generate session key-value pairs, 
            // add session information and login token information to Redis
            std::string ssid = UUID();
            _redis_session->Append(ssid, user->User_Id());
            _redis_status->Append(user->User_Id());

            response->set_request_id(rid);
            response->set_login_session_id(ssid);
            response->set_success(true);
        }

        virtual void GetPhoneVerifyCode(::google::protobuf::RpcController *controller,
                                        const ::SnowK::PhoneVerifyCodeReq *request,
                                        ::SnowK::PhoneVerifyCodeRsp *response,
                                        ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("Received request");
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string phone = request->phone_number();
            if (Phone_Check(phone) == false)
            {
                return Err_Response<::SnowK::PhoneVerifyCodeRsp>(response, rid,
                        "The mobile phone number is in the wrong format");
            }

            std::string code_id = UUID();
            std::string code = VerifyCode();
            if (_dms_client->Send(phone, code) == false)
            {
                return Err_Response<::SnowK::PhoneVerifyCodeRsp>(response, rid,
                        "The mobile phone number is in the wrong format");
            }

            _redis_codes->Append(code_id, code);

            response->set_request_id(rid);
            response->set_success(true);
            response->set_verify_code_id(code_id);
        }

        virtual void PhoneRegister(::google::protobuf::RpcController *controller,
                                   const ::SnowK::PhoneRegisterReq *request,
                                   ::SnowK::PhoneRegisterRsp *response,
                                   ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("Received request");
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string phone = request->phone_number();
            std::string code_id = request->verify_code_id();
            std::string code = request->verify_code();

            if (Phone_Check(phone) == false)
            {
                return Err_Response<::SnowK::PhoneRegisterRsp>(response, rid,
                        "The mobile phone number is in the wrong format");
            }

            // TODO OptionalString?
            if (_redis_codes->Code(code_id) != code)
            {
                return Err_Response<::SnowK::PhoneRegisterRsp>(response, rid,
                        "The verification code is incorrect");
            }

            if (_mysql_user->Select_By_Phone(phone))
            {
                return Err_Response<::SnowK::PhoneRegisterRsp>(response, rid,
                        "The mobile phone number has already been registered as a user");
            }

            std::string uid = UUID();
            auto user = std::make_shared<User>(uid, phone);
            if (_mysql_user->Insert(user) == false)
            {
                return Err_Response<::SnowK::PhoneRegisterRsp>(response, rid,
                        "Failed to add user information to the database");
            }

            if (_es_user->AppendData(uid, phone, uid, "", "") == false)
            {
                return Err_Response<::SnowK::PhoneRegisterRsp>(response, rid,
                        "Failed to add user information to the es server");
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

        virtual void PhoneLogin(::google::protobuf::RpcController *controller,
                                const ::SnowK::PhoneLoginReq *request,
                                ::SnowK::PhoneLoginRsp *response,
                                ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("Received request");
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string phone = request->phone_number();
            std::string code_id = request->verify_code_id();
            std::string code = request->verify_code();
            if (Phone_Check(phone) == false)
            {
                return Err_Response<::SnowK::PhoneLoginRsp>(response, rid,
                        "The mobile phone number is in the wrong format");
            }

            auto user = _mysql_user->Select_By_Phone(phone);
            if (!user)
            {
                return Err_Response<::SnowK::PhoneLoginRsp>(response, rid,
                        "The mobile phone number is not a registered user");
            }

            if (_redis_codes->Code(code_id) != code)
            {
                return Err_Response<::SnowK::PhoneLoginRsp>(response, rid,
                        "The verification code is incorrect");
            }
            _redis_codes->Remove(code_id);

            if (_redis_status->Exists(user->User_Id()))
            {
                return Err_Response<::SnowK::PhoneLoginRsp>(response, rid,
                        "The user is already logged in elsewhere");
            }

            std::string ssid = UUID();
            _redis_session->Append(ssid, user->User_Id());
            _redis_status->Append(user->User_Id());

            response->set_request_id(rid);
            response->set_login_session_id(ssid);
            response->set_success(true);
        }

        virtual void GetUserInfo(::google::protobuf::RpcController *controller,
                                 const ::SnowK::GetUserInfoReq *request,
                                 ::SnowK::GetUserInfoRsp *response,
                                 ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("Received request");
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();
            auto user = _mysql_user->Select_By_Id(uid);
            if (!user)
            {
                return Err_Response<::SnowK::GetUserInfoRsp>(response, rid,
                        "User information not found");
            }

            UserInfo *user_info = response->mutable_user_info();
            user_info->set_user_id(user->User_Id());
            user_info->set_nickname(user->Nickname());
            user_info->set_description(user->Description());
            user_info->set_phone(user->Phone());

            if (!user->Avatar_Id().empty())
            {
                auto channel = _svrmgr_channels->Choose(_file_service_name);
                if (!channel)
                {
                    return Err_Response<::SnowK::GetUserInfoRsp>(response, rid,
                            "The File Management subservice node was not found");
                }

                SnowK::FileService_Stub stub(channel.get());
                brpc::Controller cntl;
                SnowK::GetSingleFileReq req;
                SnowK::GetSingleFileRsp rsp;
                req.set_request_id(request->request_id());
                req.set_file_id(user->Avatar_Id());

                stub.GetSingleFile(&cntl, &req, &rsp, nullptr);
                if (cntl.Failed() == true || rsp.success() == false)
                {
                    return Err_Response<::SnowK::GetUserInfoRsp>(response, rid,
                            "File subservice call failed");
                }
                user_info->set_avatar(rsp.file_data().file_content());
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

        virtual void GetMultiUserInfo(::google::protobuf::RpcController *controller,
                                      const ::SnowK::GetMultiUserInfoReq *request,
                                      ::SnowK::GetMultiUserInfoRsp *response,
                                      ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("Received request");
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();

            std::vector<std::string> uid_lists;
            for (int i = 0; i < request->users_id_size(); i++)
            {
                uid_lists.push_back(request->users_id(i));
            }

            auto users = _mysql_user->Select_Multi_Users(uid_lists);
            if (users.size() != request->users_id_size())
            {
                return Err_Response<::SnowK::GetMultiUserInfoRsp>(response, rid,
                        "The number of user information looked up from the database is inconsistent");
            }

            auto channel = _svrmgr_channels->Choose(_file_service_name);
            if (!channel)
            {
                return Err_Response<::SnowK::GetMultiUserInfoRsp>(response, rid,
                        "The File Management subservice node was not found");
            }

            SnowK::FileService_Stub stub(channel.get());
            brpc::Controller cntl;
            SnowK::GetMultiFileReq req;
            SnowK::GetMultiFileRsp rsp;
            req.set_request_id(request->request_id());
            for (auto &user : users)
            {
                if (user.Avatar_Id().empty())
                {
                    continue;
                }

                req.add_file_id_list(user.Avatar_Id());
            }

            stub.GetMultiFile(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed() == true || rsp.success() == false)
            {
                return Err_Response<::SnowK::GetMultiUserInfoRsp>(response, rid,
                        "File subservice call failed");
            }

            for (auto &user : users)
            {
                auto user_map = response->mutable_users_info();
                auto file_map = rsp.mutable_file_data();

                UserInfo user_info;
                user_info.set_user_id(user.User_Id());
                user_info.set_nickname(user.Nickname());
                user_info.set_description(user.Description());
                user_info.set_phone(user.Phone());
                user_info.set_avatar((*file_map)[user.Avatar_Id()].file_content());

                (*user_map)[user_info.user_id()] = user_info;
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

        virtual void SetUserAvatar(::google::protobuf::RpcController *controller,
                                   const ::SnowK::SetUserAvatarReq *request,
                                   ::SnowK::SetUserAvatarRsp *response,
                                   ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("Received request");
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();
            auto user = _mysql_user->Select_By_Id(uid);
            if (!user)
            {
                return Err_Response<::SnowK::SetUserAvatarRsp>(response, rid,
                        "User information not found");
            }

            auto channel = _svrmgr_channels->Choose(_file_service_name);
            if (!channel)
            {
                return Err_Response<::SnowK::SetUserAvatarRsp>(response, rid,
                        "The File Management subservice node was not found");
            }

            SnowK::FileService_Stub stub(channel.get());
            brpc::Controller cntl;
            SnowK::PutSingleFileReq req;
            SnowK::PutSingleFileRsp rsp;
            req.set_request_id(request->request_id());
            req.mutable_file_data()->set_file_name("");
            req.mutable_file_data()->set_file_size(request->avatar().size());
            req.mutable_file_data()->set_file_content(request->avatar());

            stub.PutSingleFile(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed() == true || rsp.success() == false)
            {
                return Err_Response<::SnowK::SetUserAvatarRsp>(response, rid,
                        "File subservice call failed");
            }

            std::string avatar_id = rsp.file_info().file_id();
            user->Avatar_Id(avatar_id);
            if (_mysql_user->Update(user) == false)
            {
                return Err_Response<::SnowK::SetUserAvatarRsp>(response, rid,
                        "Failed to update user avatar ID to database");
            }

            if (_es_user->AppendData(user->User_Id(), user->Phone(),
                                     user->Nickname(), user->Description(), user->Avatar_Id()) == false)
            {
                return Err_Response<::SnowK::SetUserAvatarRsp>(response, rid,
                        "Failed to update user avatar ID to search engine");
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

        virtual void SetUserNickname(::google::protobuf::RpcController *controller,
                                     const ::SnowK::SetUserNicknameReq *request,
                                     ::SnowK::SetUserNicknameRsp *response,
                                     ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("Received request");
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string new_nickname = request->nickname();
            if (Nickname_Check(new_nickname) == false)
            {
                return Err_Response<::SnowK::SetUserNicknameRsp>(response, rid,
                        "The username length is invalid");
            }

            auto user = _mysql_user->Select_By_Id(uid);
            if (!user)
            {
                return Err_Response<::SnowK::SetUserNicknameRsp>(response, rid,
                        "User information not found");
            }

            user->Nickname(new_nickname);
            if (_mysql_user->Update(user) == false)
            {
                return Err_Response<::SnowK::SetUserNicknameRsp>(response, rid,
                        "Failed to update user nickname to database");
            }

            if (_es_user->AppendData(user->User_Id(), user->Phone(),
                                     user->Nickname(), user->Description(), user->Avatar_Id()) == false)
            {
                return Err_Response<::SnowK::SetUserNicknameRsp>(response, rid,
                        "Failed to update user nickname to search engine");
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

        virtual void SetUserDescription(::google::protobuf::RpcController *controller,
                                        const ::SnowK::SetUserDescriptionReq *request,
                                        ::SnowK::SetUserDescriptionRsp *response,
                                        ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("Received request");
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string new_description = request->description();

            auto user = _mysql_user->Select_By_Id(uid);
            if (!user)
            {
                return Err_Response<::SnowK::SetUserDescriptionRsp>(response, rid,
                        "User information not found");
            }

            user->Description(new_description);
            if (_mysql_user->Update(user) == false)
            {
                return Err_Response<::SnowK::SetUserDescriptionRsp>(response, rid,
                        "Failed to update user description to database");
            }

            if (_es_user->AppendData(user->User_Id(), user->Phone(),
                                     user->Nickname(), user->Description(), user->Avatar_Id()) == false)
            {
                return Err_Response<::SnowK::SetUserDescriptionRsp>(response, rid,
                        "Failed to update user description to search engine");
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

        virtual void SetUserPhoneNumber(::google::protobuf::RpcController *controller,
                                        const ::SnowK::SetUserPhoneNumberReq *request,
                                        ::SnowK::SetUserPhoneNumberRsp *response,
                                        ::google::protobuf::Closure *done)
        {
            LOG_DEBUG("Received request");
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string new_phone = request->phone_number();
            std::string code = request->phone_verify_code();
            std::string code_id = request->phone_verify_code_id();

            if (_redis_codes->Code(code_id) != code)
            {
                return Err_Response<::SnowK::SetUserPhoneNumberRsp>(response, rid,
                        "The verification code is incorrect");
            }

            auto user = _mysql_user->Select_By_Id(uid);
            if (!user)
            {
                return Err_Response<::SnowK::SetUserPhoneNumberRsp>(response, rid,
                        "User information not found");
            }

            user->Phone(new_phone);
            if (_mysql_user->Update(user) == false)
            {
                return Err_Response<::SnowK::SetUserPhoneNumberRsp>(response, rid,
                        "Failed to update user phone to database");
            }

            if (_es_user->AppendData(user->User_Id(), user->Phone(),
                                     user->Nickname(), user->Description(), user->Avatar_Id()) == false)
            {
                return Err_Response<::SnowK::SetUserPhoneNumberRsp>(response, rid,
                        "Failed to update user phone to search engine");
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

    private:
        bool Nickname_Check(const std::string &nickname)
        {
            return nickname.size() < 22;
        }

        bool Password_Check(const std::string &password)
        {
            if (password.size() < 6 || password.size() > 15)
            {
                return false;
            }

            for (int i = 0; i < password.size(); i++)
            {
                if (!(std::isalnum(password[i]) || password[i] == '_' || password[i] == '-'))
                {
                    return false;
                }
            }

            return true;
        }

        bool Phone_Check(const std::string &phone)
        {
            if (phone.size() != 11)
                return false;
            if (phone[0] != '1')
                return false;
            if (phone[1] < '3' || phone[1] > '9')
                return false;
                
            for (int i = 2; i < 11; i++)
            {
                if (!std::isdigit(phone[i]))
                    return false;
            }

            return true;
        }

    private:
        ESUser::ptr _es_user;
        UserTable::ptr _mysql_user;
        Session::ptr _redis_session;
        Status::ptr _redis_status;
        Codes::ptr _redis_codes;

        std::string _file_service_name;
        ServiceManager::ptr _svrmgr_channels;

        DMSClient::ptr _dms_client;
    }; // end of UserServiceImpl

    class UserServer
    {
    public:
        using ptr = std::shared_ptr<UserServer>;

        UserServer(const Discovery::ptr service_discoverer,
                   const Registry::ptr &reg_client,
                   const std::shared_ptr<elasticlient::Client> &es_client,
                   const std::shared_ptr<odb::core::database> &mysql_client,
                   const std::shared_ptr<sw::redis::Redis> &redis_client,
                   const std::shared_ptr<brpc::Server> &server)
            : _service_discoverer(service_discoverer)
            , _registry_client(reg_client)
            , _es_client(es_client)
            , _mysql_client(mysql_client)
            , _redis_client(redis_client)
            , _rpc_server(server) 
        {}
        ~UserServer() {}

        void Start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }

    private:
        Discovery::ptr _service_discoverer;
        Registry::ptr _registry_client;
        std::shared_ptr<elasticlient::Client> _es_client;
        std::shared_ptr<odb::core::database> _mysql_client;
        std::shared_ptr<sw::redis::Redis> _redis_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    }; // end of UserServer

    // Builder Pattern
    class UserServerBuilder
    {
    public:
        void Make_Es_Object(const std::vector<std::string> host_list)
        {
            _es_client = ESClientFactory::Create(host_list);
        }

        void Make_Dms_Object(const std::string &access_key_id,
                             const std::string &access_key_secret)
        {
            _dms_client = std::make_shared<DMSClient>(access_key_id, access_key_secret);
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

        void Make_Redis_Object(const std::string &host,
                               int port,
                               int db,
                               bool keep_alive)
        {
            _redis_client = RedisClientFactory::Create(host, port, db, keep_alive);
        }

        // TODO
        void Make_Discovery_Object(const std::string &reg_host,
                                   const std::string &base_service_name,
                                   const std::string &file_service_name)
        {
            _file_service_name = file_service_name;
            _svrmgr_channels = std::make_shared<ServiceManager>();
            _svrmgr_channels->Declare(file_service_name);

            LOG_DEBUG("Set the file sub-service as the sub-service to be added and managed: {}", file_service_name);

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
            if (!_redis_client)
            {
                LOG_ERROR("The Redis database module has not been initialized");
                abort();
            }
            if (!_svrmgr_channels)
            {
                LOG_ERROR("The channel management module has not been initialized");
                abort();
            }
            if (!_dms_client)
            {
                LOG_ERROR("The SMS platform module has not been initialized");
                abort();
            }

            _rpc_server = std::make_shared<brpc::Server>();

            UserServiceImpl *user_service = new UserServiceImpl(_dms_client, _es_client,
                                                                _mysql_client, _redis_client, 
                                                                _svrmgr_channels, _file_service_name);
            if (_rpc_server->AddService(user_service, brpc::ServiceOwnership::SERVER_OWNS_SERVICE) == -1)
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

        UserServer::ptr Build()
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

            UserServer::ptr server = std::make_shared<UserServer>(
                _service_discoverer, _registry_client,
                _es_client, _mysql_client, _redis_client, _rpc_server);
                
            return server;
        }

    private:
        Registry::ptr _registry_client;

        std::shared_ptr<elasticlient::Client> _es_client;
        std::shared_ptr<odb::core::database> _mysql_client;
        std::shared_ptr<sw::redis::Redis> _redis_client;

        std::string _file_service_name;
        ServiceManager::ptr _svrmgr_channels;
        Discovery::ptr _service_discoverer;

        std::shared_ptr<DMSClient> _dms_client;

        std::shared_ptr<brpc::Server> _rpc_server;
    }; // end of UserServerBuilder
}