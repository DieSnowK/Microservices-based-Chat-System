#include "httplib.h"
#include "data_redis.hpp"
#include "etcd.hpp"
#include "logger.hpp"
#include "channel.hpp"
#include "connections.hpp"
#include "redis.hpp"

#include "user.pb.h"
#include "base.pb.h"
#include "file.pb.h"
#include "friend.pb.h"
#include "gateway.pb.h"
#include "message.pb.h"
#include "speech.pb.h"
#include "transmite.pb.h"
#include "notify.pb.h"

#define GET_PHONE_VERIFY_CODE "/service/user/get_phone_verify_code"
#define USERNAME_REGISTER "/service/user/username_register"
#define USERNAME_LOGIN "/service/user/username_login"
#define PHONE_REGISTER "/service/user/phone_register"
#define PHONE_LOGIN "/service/user/phone_login"
#define GET_USERINFO "/service/user/get_user_info"
#define SET_USER_AVATAR "/service/user/set_avatar"
#define SET_USER_NICKNAME "/service/user/set_nickname"
#define SET_USER_DESC "/service/user/set_description"
#define SET_USER_PHONE "/service/user/set_phone"
#define FRIEND_GET_LIST "/service/friend/get_friend_list"
#define FRIEND_APPLY "/service/friend/add_friend_apply"
#define FRIEND_APPLY_PROCESS "/service/friend/add_friend_process"
#define FRIEND_REMOVE "/service/friend/remove_friend"
#define FRIEND_SEARCH "/service/friend/search_friend"
#define FRIEND_GET_PENDING_EV "/service/friend/get_pending_friend_events"
#define CSS_GET_LIST "/service/friend/get_chat_session_list"
#define CSS_CREATE "/service/friend/create_chat_session"
#define CSS_GET_MEMBER "/service/friend/get_chat_session_member"
#define MSG_GET_RANGE "/service/message_storage/get_history"
#define MSG_GET_RECENT "/service/message_storage/get_recent"
#define MSG_KEY_SEARCH "/service/message_storage/search_history"
#define NEW_MESSAGE "/service/message_transmit/new_message"
#define FILE_GET_SINGLE "/service/file/get_single_file"
#define FILE_GET_MULTI "/service/file/get_multi_file"
#define FILE_PUT_SINGLE "/service/file/put_single_file"
#define FILE_PUT_MULTI "/service/file/put_multi_file"
#define SPEECH_RECOGNITION "/service/speech/recognition"

namespace SnowK
{
    class GatewayServer
    {
    public:
        using ptr = std::shared_ptr<GatewayServer>;

        GatewayServer(int websocket_port, int http_port,
                      const std::shared_ptr<sw::redis::Redis> &redis_client,
                      const ServiceManager::ptr &channels,
                      const Discovery::ptr &service_discoverer,
                      const std::string user_service_name,
                      const std::string file_service_name,
                      const std::string speech_service_name,
                      const std::string message_service_name,
                      const std::string transmite_service_name,
                      const std::string friend_service_name)
            : _redis_session(std::make_shared<Session>(redis_client))
            , _redis_status(std::make_shared<Status>(redis_client))
            , _svrmgr_channels(channels)
            , _service_discoverer(service_discoverer)
            , _user_service_name(user_service_name)
            , _file_service_name(file_service_name)
            , _speech_service_name(speech_service_name)
            , _message_service_name(message_service_name)
            , _transmite_service_name(transmite_service_name)
            , _friend_service_name(friend_service_name)
            , _connections(std::make_shared<Connections>())
        {
            _ws_server.set_access_channels(websocketpp::log::alevel::none);
            _ws_server.init_asio();

            _ws_server.set_open_handler(std::bind(&GatewayServer::WsOnOpen, this, std::placeholders::_1));
            _ws_server.set_close_handler(std::bind(&GatewayServer::WsOnClose, this, std::placeholders::_1));
            _ws_server.set_message_handler(std::bind(&GatewayServer::WsOnMessage, this,
                                                     std::placeholders::_1, std::placeholders::_2));
            _ws_server.set_reuse_addr(true);
            _ws_server.listen(websocket_port);
            _ws_server.start_accept();

            // Routings
            _http_server.Post(GET_PHONE_VERIFY_CODE,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::GetPhoneVerifyCode, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(USERNAME_REGISTER,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::UserRegister, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(USERNAME_LOGIN,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::UserLogin, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(PHONE_REGISTER,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::PhoneRegister, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(PHONE_LOGIN,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::PhoneLogin, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(GET_USERINFO,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::GetUserInfo, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(SET_USER_AVATAR,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::SetUserAvatar, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(SET_USER_NICKNAME,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::SetUserNickname, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(SET_USER_DESC,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::SetUserDescription, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(SET_USER_PHONE,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::SetUserPhoneNumber, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(FRIEND_GET_LIST,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::GetFriendList, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(FRIEND_APPLY,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::FriendAdd, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(FRIEND_APPLY_PROCESS,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::FriendAddProcess, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(FRIEND_REMOVE,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::FriendRemove, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(FRIEND_SEARCH,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::FriendSearch, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(FRIEND_GET_PENDING_EV,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::GetPendingFriendEventList, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(CSS_GET_LIST,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::GetChatSessionList, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(CSS_CREATE,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::ChatSessionCreate, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(CSS_GET_MEMBER,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::GetChatSessionMember, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(MSG_GET_RANGE,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::GetHistoryMsg, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(MSG_GET_RECENT,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::GetRecentMsg, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(MSG_KEY_SEARCH,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::MsgSearch, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(NEW_MESSAGE,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::NewMessage, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(FILE_GET_SINGLE,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::GetSingleFile, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(FILE_GET_MULTI,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::GetMultiFile, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(FILE_PUT_SINGLE,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::PutSingleFile, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(FILE_PUT_MULTI,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::PutMultiFile, this,
                                std::placeholders::_1, std::placeholders::_2));
            _http_server.Post(SPEECH_RECOGNITION,
                                (httplib::Server::Handler)std::bind(
                                &GatewayServer::SpeechRecognition, this,
                                std::placeholders::_1, std::placeholders::_2));

            _http_thread = std::thread([this, http_port]()
                                       { _http_server.listen("0.0.0.0", http_port); });
            _http_thread.detach();
        }

        void Start()
        {
            _ws_server.run();
        }

    private:
        template <class T>
        void Err_Response(T &rsp, httplib::Response &response, const std::string &errmsg)
        {
            rsp.set_success(false);
            rsp.set_errmsg(errmsg);
            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");

            LOG_ERROR("{}", errmsg);
        }

        // ※ Very important, very easy to ignore
        void KeepAlive(server_t::connection_ptr conn)
        {
            if (!conn || conn->get_state() != websocketpp::session::state::value::open)
            {
                LOG_WARN("Abnormal connection status, end connection keep-alive");
                return;
            }

            conn->ping("");
            _ws_server.set_timer(60000, std::bind(&GatewayServer::KeepAlive, this, conn));
        }

        // TODO 是否这里改为WsOnMessage的功能?  等待客户端接口验证
        void WsOnOpen(websocketpp::connection_hdl hdl)
        {
            LOG_DEBUG("A persistent websocket connection is established {}", 
                      (size_t)_ws_server.get_con_from_hdl(hdl).get());
        }

        // Clean up when a persistent connection is lost
        void WsOnClose(websocketpp::connection_hdl hdl)
        {
            auto conn = _ws_server.get_con_from_hdl(hdl);
            std::string uid, ssid;
            if (_connections->GetClientInfo(conn, uid, ssid) == false)
            {
                LOG_WARN("The persistent connection is disconnected, and the client \
                         information corresponding to the persistent connection cannot be found");
                return;
            }

            _redis_session->Remove(ssid);
            _redis_status->Remove(uid);
            _connections->Remove(conn);

            LOG_DEBUG("{} {} {} the persistent connection is disconnected, \
                      the cache data will be cleared", ssid, uid, (size_t)conn.get());
        }

        // After receiving the first message, the client persists the 
        // client connection for management based on the session ID in the message
        void WsOnMessage(websocketpp::connection_hdl hdl, server_t::message_ptr msg)
        {
            auto conn = _ws_server.get_con_from_hdl(hdl);

            ClientAuthenticationReq request;
            if (request.ParseFromString(msg->get_payload()) == false)
            {
                LOG_ERROR("Persistent connection identification failure: The body deserialization fails");
                _ws_server.close(hdl, websocketpp::close::status::unsupported_data, "Body deserialization failed");
                return;
            }

            std::string ssid = request.session_id();
            auto uid = _redis_session->Uid(ssid);

            // If not exists
            if (!uid)
            {
                LOG_ERROR("Persistent connection identification failed: \
                          No session information found {}", ssid);
                _ws_server.close(hdl, websocketpp::close::status::unsupported_data, 
                                 "Session information not found");
                return;
            }

            // If exists
            _connections->Insert(conn, *uid, ssid);
            KeepAlive(conn);
            LOG_DEBUG("New Persistent connection: {} - {} - {}", ssid, *uid, (size_t)conn.get());
        }

        void GetPhoneVerifyCode(const httplib::Request &request, httplib::Response &response)
        {
            PhoneVerifyCodeReq req;
            PhoneVerifyCodeRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<PhoneVerifyCodeRsp>(rsp, response, 
                    "Failed to deserialize the request body to obtain the SMS verification code");
            }

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                return Err_Response<PhoneVerifyCodeRsp>(rsp, response,
                        "No user sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.GetPhoneVerifyCode(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<PhoneVerifyCodeRsp>(rsp, response,
                        "User sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void UserRegister(const httplib::Request &request, httplib::Response &response)
        {
            UserRegisterReq req;
            UserRegisterRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<UserRegisterRsp>(rsp, response,
                        "Failed to deserialize the user name registration request body");
            }

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                return Err_Response<UserRegisterRsp>(rsp, response,
                        "No user sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.UserRegister(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<UserRegisterRsp>(rsp, response,
                        "User sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void UserLogin(const httplib::Request &request, httplib::Response &response)
        {
            UserLoginReq req;
            UserLoginRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<UserLoginRsp>(rsp, response,
                        "Failed to deserialize user login request body");
            }

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                return Err_Response<UserLoginRsp>(rsp, response,
                        "No user sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.UserLogin(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<UserLoginRsp>(rsp, response,
                        "User sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void PhoneRegister(const httplib::Request &request, httplib::Response &response)
        {
            PhoneRegisterReq req;
            PhoneRegisterRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<PhoneRegisterRsp>(rsp, response,
                        "Failed to deserialize the body of the mobile phone number registration request");
            }

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                return Err_Response<PhoneRegisterRsp>(rsp, response,
                        "No user sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.PhoneRegister(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<PhoneRegisterRsp>(rsp, response,
                        "User sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void PhoneLogin(const httplib::Request &request, httplib::Response &response)
        {
            PhoneLoginReq req;
            PhoneLoginRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<PhoneLoginRsp>(rsp, response,
                        "Failed to deserialize the body of the mobile phone number login request");
            }

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                return Err_Response<PhoneLoginRsp>(rsp, response,
                        "No user sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.PhoneLogin(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<PhoneLoginRsp>(rsp, response,
                        "User sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        // Authentication is required to start with the following APIs:
        void GetUserInfo(const httplib::Request &request, httplib::Response &response)
        {
            GetUserInfoReq req;
            GetUserInfoRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<GetUserInfoRsp>(rsp, response,
                        "Failed to deserialize the body of the request to get user information");
            }

            // Client Identification and Authentication
            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<GetUserInfoRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                return Err_Response<GetUserInfoRsp>(rsp, response,
                        "No user sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.GetUserInfo(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<GetUserInfoRsp>(rsp, response,
                        "User sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void SetUserAvatar(const httplib::Request &request, httplib::Response &response)
        {
            SetUserAvatarReq req;
            SetUserAvatarRsp rsp;

            bool ret = req.ParseFromString(request.body);
            if (ret == false)
            {
                return Err_Response<SetUserAvatarRsp>(rsp, response,
                        "Failed to deserialize the body of the user avatar setting request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<SetUserAvatarRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                return Err_Response<SetUserAvatarRsp>(rsp, response,
                        "No user sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.SetUserAvatar(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<SetUserAvatarRsp>(rsp, response,
                        "User sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void SetUserNickname(const httplib::Request &request, httplib::Response &response)
        {
            SetUserNicknameReq req;
            SetUserNicknameRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<SetUserNicknameRsp>(rsp, response,
                        "Failed to deserialize the body of user nickname setting request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<SetUserNicknameRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                return Err_Response<SetUserNicknameRsp>(rsp, response,
                        "No user sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.SetUserNickname(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<SetUserNicknameRsp>(rsp, response,
                        "User sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void SetUserDescription(const httplib::Request &request, httplib::Response &response)
        {
            SetUserDescriptionReq req;
            SetUserDescriptionRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<SetUserDescriptionRsp>(rsp, response,
                    "Failed to deserialize the body of the user signature setting request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<SetUserDescriptionRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                return Err_Response<SetUserDescriptionRsp>(rsp, response,
                        "No user sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.SetUserDescription(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<SetUserDescriptionRsp>(rsp, response,
                        "User sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void SetUserPhoneNumber(const httplib::Request &request, httplib::Response &response)
        {
            SetUserPhoneNumberReq req;
            SetUserPhoneNumberRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<SetUserPhoneNumberRsp>(rsp, response,
                        "Failed to deserialize the body of the request for setting the user's mobile phone number");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<SetUserPhoneNumberRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                return Err_Response<SetUserPhoneNumberRsp>(rsp, response,
                        "No user sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.SetUserPhoneNumber(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<SetUserPhoneNumberRsp>(rsp, response,
                        "User sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void GetFriendList(const httplib::Request &request, httplib::Response &response)
        {
            GetFriendListReq req;
            GetFriendListRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<GetFriendListRsp>(rsp, response,
                        "Failed to deserialize the body of the get friends list request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<GetFriendListRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_friend_service_name);
            if (!channel)
            {
                return Err_Response<GetFriendListRsp>(rsp, response,
                        "No friend sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FriendService_Stub stub(channel.get());
            stub.GetFriendList(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<GetFriendListRsp>(rsp, response,
                        "Friend sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        std::shared_ptr<GetUserInfoRsp> _GetUserInfo(const std::string &rid, const std::string &uid)
        {
            GetUserInfoReq req;
            auto rsp = std::make_shared<GetUserInfoRsp>();
            req.set_request_id(rid);
            req.set_user_id(uid);

            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                LOG_ERROR("{} No user sub-service node found to provide business processing", req.request_id());
                return std::shared_ptr<GetUserInfoRsp>();
            }

            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.GetUserInfo(&cntl, &req, rsp.get(), nullptr);
            if (cntl.Failed())
            {
                LOG_ERROR("{} User sub-service call failed", req.request_id());
                return std::shared_ptr<GetUserInfoRsp>();
            }

            return rsp;
        }

        // 1.In the business processing of friend requests, the friend sub-service
            // actually only creates a request event in the database
        // 2.What the gateway needs to do: When the friend sub-service completes the business 
            // processing, if the processing is successful, the applicant needs to be notified
                // websocket is needed
        void FriendAdd(const httplib::Request &request, httplib::Response &response)
        {
            FriendAddReq req;
            FriendAddRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<FriendAddRsp>(rsp, response,
                        "Failed to deserialize the body of the add friend request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<FriendAddRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_friend_service_name);
            if (!channel)
            {
                return Err_Response<FriendAddRsp>(rsp, response,
                        "No friend sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FriendService_Stub stub(channel.get());
            stub.FriendAdd(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<FriendAddRsp>(rsp, response,
                        "Friend sub-service call failed");
            }

            // Notify the client -> websocket
            auto conn = _connections->GetConnection(req.respondent_id());
            if (rsp.success() && conn)
            {
                auto user_rsp = _GetUserInfo(req.request_id(), *uid);
                if (!user_rsp)
                {
                    return Err_Response<FriendAddRsp>(rsp, response,
                            "Failed to get the current client user information");
                }

                NotifyMessage notify;
                notify.set_notify_type(NotifyType::FRIEND_ADD_APPLY_NOTIFY);
                notify.mutable_friend_add_apply()->mutable_user_info()->CopyFrom(user_rsp->user_info());

                conn->send(notify.SerializeAsString(), websocketpp::frame::opcode::value::binary);
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void FriendAddProcess(const httplib::Request &request, httplib::Response &response)
        {
            FriendAddProcessReq req;
            FriendAddProcessRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<FriendAddProcessRsp>(rsp, response,
                        "Failed to deserialize the body of the friend application processing request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<FriendAddProcessRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_friend_service_name);
            if (!channel)
            {
                return Err_Response<FriendAddProcessRsp>(rsp, response,
                        "No friend sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FriendService_Stub stub(channel.get());
            stub.FriendAddProcess(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<FriendAddProcessRsp>(rsp, response,
                        "Friend sub-service call failed");
            }

            if (rsp.success())
            {
                auto process_user_rsp = _GetUserInfo(req.request_id(), *uid);
                auto apply_user_rsp = _GetUserInfo(req.request_id(), req.apply_user_id());
                if (!process_user_rsp || !apply_user_rsp)
                {
                    return Err_Response<FriendAddProcessRsp>(rsp, response,
                            "Friend sub-service call failed");
                }

                auto process_conn = _connections->GetConnection(*uid);
                auto apply_conn = _connections->GetConnection(req.apply_user_id());

                // Notify the applicant, what ever agree or not :P
                if (apply_conn)
                {
                    NotifyMessage notify;
                    notify.set_notify_type(NotifyType::FRIEND_ADD_PROCESS_NOTIFY);
                    auto process_result = notify.mutable_friend_process_result();
                    process_result->mutable_user_info()->CopyFrom(process_user_rsp->user_info());
                    process_result->set_agree(req.agree());
                    
                    apply_conn->send(notify.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                }

                // Notify both parties of the session creation
                // Session information is processor information
                if (req.agree() && apply_conn)
                {
                    NotifyMessage notify;
                    notify.set_notify_type(NotifyType::CHAT_SESSION_CREATE_NOTIFY);
                    auto new_chat_session = notify.mutable_new_chat_session_info();
                    auto chat_session_info = new_chat_session->mutable_chat_session_info();

                    chat_session_info->set_single_chat_friend_id(*uid);
                    chat_session_info->set_chat_session_id(rsp.new_session_id());
                    chat_session_info->set_chat_session_name(process_user_rsp->user_info().nickname());
                    chat_session_info->set_avatar(process_user_rsp->user_info().avatar());

                    apply_conn->send(notify.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                }

                // Session information is applicant information
                if (req.agree() && process_conn)
                {
                    NotifyMessage notify;
                    notify.set_notify_type(NotifyType::CHAT_SESSION_CREATE_NOTIFY);
                    auto new_chat_session = notify.mutable_new_chat_session_info();
                    auto chat_session_info = new_chat_session->mutable_chat_session_info();

                    chat_session_info->set_single_chat_friend_id(req.apply_user_id());
                    chat_session_info->set_chat_session_id(rsp.new_session_id());
                    chat_session_info->set_chat_session_name(apply_user_rsp->user_info().nickname());
                    chat_session_info->set_avatar(apply_user_rsp->user_info().avatar());

                    process_conn->send(notify.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                }
            } // end of if (rsp.success())

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void FriendRemove(const httplib::Request &request, httplib::Response &response)
        {
            FriendRemoveReq req;
            FriendRemoveRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<FriendRemoveRsp>(rsp, response,
                        "Failed to deserialize the body of the delete a friend request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<FriendRemoveRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_friend_service_name);
            if (!channel)
            {
                return Err_Response<FriendRemoveRsp>(rsp, response,
                        "No friend sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FriendService_Stub stub(channel.get());
            stub.FriendRemove(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<FriendRemoveRsp>(rsp, response,
                        "Friend sub-service call failed");
            }

            auto conn = _connections->GetConnection(req.peer_id());
            if (rsp.success() && conn)
            {
                NotifyMessage notify;
                notify.set_notify_type(NotifyType::FRIEND_REMOVE_NOTIFY);
                notify.mutable_friend_remove()->set_user_id(*uid);

                conn->send(notify.SerializeAsString(), websocketpp::frame::opcode::value::binary);
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void FriendSearch(const httplib::Request &request, httplib::Response &response)
        {
            FriendSearchReq req;
            FriendSearchRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<FriendSearchRsp>(rsp, response,
                        "Failed to deserialize the body of the user search request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<FriendSearchRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_friend_service_name);
            if (!channel)
            {
                return Err_Response<FriendSearchRsp>(rsp, response,
                        "No friend sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FriendService_Stub stub(channel.get());
            stub.FriendSearch(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<FriendSearchRsp>(rsp, response,
                        "Friend sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void GetPendingFriendEventList(const httplib::Request &request, httplib::Response &response)
        {
            GetPendingFriendEventListReq req;
            GetPendingFriendEventListRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<GetPendingFriendEventListRsp>(rsp, response,
                        "Failed to deserialize the body of the get pending friend request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<GetPendingFriendEventListRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_friend_service_name);
            if (!channel)
            {
                return Err_Response<GetPendingFriendEventListRsp>(rsp, response,
                        "No friend sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FriendService_Stub stub(channel.get());
            stub.GetPendingFriendEventList(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<GetPendingFriendEventListRsp>(rsp, response,
                        "Friend sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void GetChatSessionList(const httplib::Request &request, httplib::Response &response)
        {
            GetChatSessionListReq req;
            GetChatSessionListRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<GetChatSessionListRsp>(rsp, response,
                        "Failed to deserialize the body of the get chat session list request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<GetChatSessionListRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_friend_service_name);
            if (!channel)
            {
                return Err_Response<GetChatSessionListRsp>(rsp, response,
                        "No friend sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FriendService_Stub stub(channel.get());
            stub.GetChatSessionList(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<GetChatSessionListRsp>(rsp, response,
                        "Friend sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void GetChatSessionMember(const httplib::Request &request, httplib::Response &response)
        {
            GetChatSessionMemberReq req;
            GetChatSessionMemberRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<GetChatSessionMemberRsp>(rsp, response,
                        "Failed to deserialize the body of the get chat session member request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<GetChatSessionMemberRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_friend_service_name);
            if (!channel)
            {
                return Err_Response<GetChatSessionMemberRsp>(rsp, response,
                        "No friend sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FriendService_Stub stub(channel.get());
            stub.GetChatSessionMember(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<GetChatSessionMemberRsp>(rsp, response,
                        "Friend sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void ChatSessionCreate(const httplib::Request &request, httplib::Response &response)
        {
            ChatSessionCreateReq req;
            ChatSessionCreateRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<ChatSessionCreateRsp>(rsp, response,
                        "Failed to deserialize the body of the create chat session request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<ChatSessionCreateRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_friend_service_name);
            if (!channel)
            {
                return Err_Response<ChatSessionCreateRsp>(rsp, response,
                        "No friend sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FriendService_Stub stub(channel.get());
            stub.ChatSessionCreate(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<ChatSessionCreateRsp>(rsp, response,
                        "Friend sub-service call failed");
            }

            if (rsp.success())
            {
                for (int i = 0; i < req.member_id_list_size(); i++)
                {
                    auto conn = _connections->GetConnection(req.member_id_list(i));
                    if (!conn)
                    {
                        continue;
                    }

                    NotifyMessage notify;
                    notify.set_notify_type(NotifyType::CHAT_SESSION_CREATE_NOTIFY);
                    auto chat_session = notify.mutable_new_chat_session_info();
                    chat_session->mutable_chat_session_info()->CopyFrom(rsp.chat_session_info());

                    conn->send(notify.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                }
            }

            rsp.clear_chat_session_info();
            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void GetHistoryMsg(const httplib::Request &request, httplib::Response &response)
        {
            GetHistoryMsgReq req;
            GetHistoryMsgRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<GetHistoryMsgRsp>(rsp, response,
                        "Failed to deserialize the body of the get interval message request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<GetHistoryMsgRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_message_service_name);
            if (!channel)
            {
                return Err_Response<GetHistoryMsgRsp>(rsp, response,
                        "No message sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::MsgStorageService_Stub stub(channel.get());
            stub.GetHistoryMsg(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<GetHistoryMsgRsp>(rsp, response,
                        "Message sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void GetRecentMsg(const httplib::Request &request, httplib::Response &response)
        {
            GetRecentMsgReq req;
            GetRecentMsgRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<GetRecentMsgRsp>(rsp, response,
                        "Failed to deserialize the body of the get recent message request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<GetRecentMsgRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_message_service_name);
            if (!channel)
            {
                return Err_Response<GetRecentMsgRsp>(rsp, response,
                        "No message sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::MsgStorageService_Stub stub(channel.get());
            stub.GetRecentMsg(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<GetRecentMsgRsp>(rsp, response,
                        "Message sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void MsgSearch(const httplib::Request &request, httplib::Response &response)
        {
            MsgSearchReq req;
            MsgSearchRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<MsgSearchRsp>(rsp, response,
                        "Failed to deserialize the body of the message search request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<MsgSearchRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_message_service_name);
            if (!channel)
            {
                return Err_Response<MsgSearchRsp>(rsp, response,
                        "No message sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::MsgStorageService_Stub stub(channel.get());
            stub.MsgSearch(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<MsgSearchRsp>(rsp, response,
                        "Message sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void GetSingleFile(const httplib::Request &request, httplib::Response &response)
        {
            GetSingleFileReq req;
            GetSingleFileRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<GetSingleFileRsp>(rsp, response,
                        "Failed to deserialize the body of the single-file download request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<GetSingleFileRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_file_service_name);
            if (!channel)
            {
                return Err_Response<GetSingleFileRsp>(rsp, response,
                        "No file sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FileService_Stub stub(channel.get());
            stub.GetSingleFile(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<GetSingleFileRsp>(rsp, response,
                        "File sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void GetMultiFile(const httplib::Request &request, httplib::Response &response)
        {
            GetMultiFileReq req;
            GetMultiFileRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<GetMultiFileRsp>(rsp, response,
                        "Failed to deserialize the body of the multi-file download request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<GetMultiFileRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_file_service_name);
            if (!channel)
            {
                return Err_Response<GetMultiFileRsp>(rsp, response,
                        "No file sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FileService_Stub stub(channel.get());
            stub.GetMultiFile(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<GetMultiFileRsp>(rsp, response,
                        "File sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void PutSingleFile(const httplib::Request &request, httplib::Response &response)
        {
            PutSingleFileReq req;
            PutSingleFileRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<PutSingleFileRsp>(rsp, response,
                        "Failed to deserialize the body of the single-file upload request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<PutSingleFileRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_file_service_name);
            if (!channel)
            {
                return Err_Response<PutSingleFileRsp>(rsp, response,
                        "No file sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FileService_Stub stub(channel.get());
            stub.PutSingleFile(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<PutSingleFileRsp>(rsp, response,
                        "File sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void PutMultiFile(const httplib::Request &request, httplib::Response &response)
        {
            PutMultiFileReq req;
            PutMultiFileRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<PutMultiFileRsp>(rsp, response,
                        "Failed to deserialize the body of the multi-file upload request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<PutMultiFileRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_file_service_name);
            if (!channel)
            {
                return Err_Response<PutMultiFileRsp>(rsp, response,
                        "No file sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::FileService_Stub stub(channel.get());
            stub.PutMultiFile(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<PutMultiFileRsp>(rsp, response,
                        "File sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void SpeechRecognition(const httplib::Request &request, httplib::Response &response)
        {
            SpeechRecognitionReq req;
            SpeechRecognitionRsp rsp;

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<SpeechRecognitionRsp>(rsp, response,
                        "Failed to deserialize the body of the speech recognition request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<SpeechRecognitionRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_speech_service_name);
            if (!channel)
            {
                return Err_Response<SpeechRecognitionRsp>(rsp, response,
                        "No speech sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::SpeechService_Stub stub(channel.get());
            stub.SpeechRecognition(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<SpeechRecognitionRsp>(rsp, response,
                        "Speech sub-service call failed");
            }

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

        void NewMessage(const httplib::Request &request, httplib::Response &response)
        {
            NewMessageReq req;
            NewMessageRsp rsp;               // Response to the client
            GetTransmitTargetRsp target_rsp; // Response to the subservice

            if (req.ParseFromString(request.body) == false)
            {
                return Err_Response<NewMessageRsp>(rsp, response,
                        "Failed to deserialize the body of the new message request");
            }

            std::string ssid = req.session_id();
            auto uid = _redis_session->Uid(ssid);
            if (!uid)
            {
                return Err_Response<NewMessageRsp>(rsp, response,
                        "Failed to get user information associated with login session");
            }
            req.set_user_id(*uid);

            auto channel = _svrmgr_channels->Choose(_transmite_service_name);
            if (!channel)
            {
                return Err_Response<NewMessageRsp>(rsp, response,
                        "No transmite sub-service node found to provide business processing");
            }

            brpc::Controller cntl;
            SnowK::MsgTransmitService_Stub stub(channel.get());
            stub.GetTransmitTarget(&cntl, &req, &target_rsp, nullptr);
            if (cntl.Failed())
            {
                return Err_Response<NewMessageRsp>(rsp, response,
                        "File transmite-service call failed");
            }

            if (target_rsp.success())
            {
                for (int i = 0; i < target_rsp.target_id_list_size(); i++)
                {
                    std::string notify_uid = target_rsp.target_id_list(i);
                    if (notify_uid == *uid)
                    {
                        continue; // Do not inform yourself
                    }

                    auto conn = _connections->GetConnection(notify_uid);
                    if (!conn)
                    {
                        continue;
                    }

                    NotifyMessage notify;
                    notify.set_notify_type(NotifyType::CHAT_MESSAGE_NOTIFY);
                    auto msg_info = notify.mutable_new_message_info();
                    msg_info->mutable_message_info()->CopyFrom(target_rsp.message());
                    
                    conn->send(notify.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                }
            }

            rsp.set_request_id(req.request_id());
            rsp.set_success(target_rsp.success());
            rsp.set_errmsg(target_rsp.errmsg());

            response.set_content(rsp.SerializeAsString(), "application/x-protbuf");
        }

    private:
        Session::ptr _redis_session;
        Status::ptr _redis_status;

        std::string _user_service_name;
        std::string _file_service_name;
        std::string _speech_service_name;
        std::string _message_service_name;
        std::string _transmite_service_name;
        std::string _friend_service_name;

        ServiceManager::ptr _svrmgr_channels;
        Discovery::ptr _service_discoverer;

        Connections::ptr _connections;

        server_t _ws_server;
        httplib::Server _http_server;
        std::thread _http_thread;
    }; // end of GatewayServer

    // Builder Pattern
    class GatewayServerBuilder
    {
    public:
        void Make_Redis_Object(const std::string &host,
                               int port,
                               int db,
                               bool keep_alive)
        {
            _redis_client = RedisClientFactory::Create(host, port, db, keep_alive);
        }

        void Make_Discovery_Object(const std::string &reg_host,
                                   const std::string &base_service_name,
                                   const std::string &file_service_name,
                                   const std::string &speech_service_name,
                                   const std::string &message_service_name,
                                   const std::string &friend_service_name,
                                   const std::string &user_service_name,
                                   const std::string &transmite_service_name)
        {
            _file_service_name = file_service_name;
            _speech_service_name = speech_service_name;
            _message_service_name = message_service_name;
            _friend_service_name = friend_service_name;
            _user_service_name = user_service_name;
            _transmite_service_name = transmite_service_name;

            _svrmgr_channels = std::make_shared<ServiceManager>();
            _svrmgr_channels->Declare(file_service_name);
            _svrmgr_channels->Declare(speech_service_name);
            _svrmgr_channels->Declare(message_service_name);
            _svrmgr_channels->Declare(friend_service_name);
            _svrmgr_channels->Declare(user_service_name);
            _svrmgr_channels->Declare(transmite_service_name);

            auto put_cb = std::bind(&ServiceManager::ServiceOnline, _svrmgr_channels.get(), std::placeholders::_1, std::placeholders::_2);
            auto del_cb = std::bind(&ServiceManager::ServiceOnline, _svrmgr_channels.get(), std::placeholders::_1, std::placeholders::_2);

            _service_discoverer = std::make_shared<Discovery>(reg_host, base_service_name, put_cb, del_cb);
        }

        void Make_Server_Object(int websocket_port, int http_port)
        {
            _websocket_port = websocket_port;
            _http_port = http_port;
        }

        GatewayServer::ptr Build()
        {
            if (!_redis_client)
            {
                LOG_ERROR("The Redis database module has not been initialized");
                abort();
            }
            if (!_service_discoverer)
            {
                LOG_ERROR("The service discovery module has not been initialized");
                abort();
            }
            if (!_svrmgr_channels)
            {
                LOG_ERROR("The channel management module has not been initialized");
                abort();
            }

            GatewayServer::ptr server = std::make_shared<GatewayServer>(
                _websocket_port, _http_port, _redis_client, _svrmgr_channels,
                _service_discoverer, _user_service_name, _file_service_name,
                _speech_service_name, _message_service_name,
                _transmite_service_name, _friend_service_name);

            return server;
        }

    private:
        int _websocket_port;
        int _http_port;

        std::shared_ptr<sw::redis::Redis> _redis_client;

        std::string _file_service_name;
        std::string _speech_service_name;
        std::string _message_service_name;
        std::string _friend_service_name;
        std::string _user_service_name;
        std::string _transmite_service_name;

        ServiceManager::ptr _svrmgr_channels;
        Discovery::ptr _service_discoverer;
    };
}
