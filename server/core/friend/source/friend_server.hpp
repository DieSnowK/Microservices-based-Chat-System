#include <brpc/server.h>
#include <butil/logging.h>

#include "data_es.hpp"
#include "mysql_chat_session_member.hpp"
#include "mysql_chat_session.hpp"
#include "mysql_relation.hpp"
#include "mysql_apply.hpp"
#include "es.hpp"
#include "etcd.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "channel.hpp"

#include "friend.pb.h"
#include "base.pb.h"
#include "user.pb.h"
#include "message.pb.h"

namespace SnowK
{
    class FriendServiceImpl : public SnowK::FriendService
    {
    private:
        template<class T>
        void Err_Response(T* response, const std::string &rid, const std::string &errmsg)
        {
            response->set_request_id(rid);
            response->set_success(false);
            response->set_errmsg(errmsg);

            LOG_ERROR("{} - {}", rid, errmsg);
        }

    public:
        FriendServiceImpl(const std::shared_ptr<elasticlient::Client> &es_client,
                          const std::shared_ptr<odb::core::database> &mysql_client,
                          const ServiceManager::ptr &channel_manager,
                          const std::string &user_service_name,
                          const std::string &message_service_name) 
            : _es_user(std::make_shared<ESUser>(es_client))
            , _mysql_apply(std::make_shared<FriendApplyTable>(mysql_client))
            , _mysql_chat_session(std::make_shared<ChatSessionTable>(mysql_client))
            , _mysql_chat_session_member(std::make_shared<ChatSessionMemeberTable>(mysql_client))
            , _mysql_relation(std::make_shared<RelationTable>(mysql_client))
            , _svrmgr_channels(channel_manager)
            , _user_service_name(user_service_name)
            , _message_service_name(message_service_name)
             
        {}
        ~FriendServiceImpl() {}

        virtual void GetFriendList(::google::protobuf::RpcController *controller,
                                    const ::SnowK::GetFriendListReq *request,
                                    ::SnowK::GetFriendListRsp *response,
                                    ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();

            auto friend_id_lists = _mysql_relation->Friends(uid);

            std::unordered_map<std::string, UserInfo> user_list;
            if (GetUserInfo(rid, friend_id_lists, user_list) == false)
            {
                return Err_Response<::SnowK::GetFriendListRsp>(response, rid, 
                        "Failed to obtain user information in batches");
            }

            response->set_request_id(rid);
            response->set_success(true);
            for (const auto &user_it : user_list)
            {
                auto user_info = response->add_friend_list();
                user_info->CopyFrom(user_it.second);
            }
        }

        virtual void FriendRemove(::google::protobuf::RpcController *controller,
                                    const ::SnowK::FriendRemoveReq *request,
                                    ::SnowK::FriendRemoveRsp *response,
                                    ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string pid = request->peer_id();
            
            if (_mysql_relation->Remove(uid, pid) == false)
            {
                return Err_Response<::SnowK::FriendRemoveRsp>(response, rid, 
                        "Failed to delete friend information from the database");
            }

            if (_mysql_chat_session->Remove(uid, pid) == false)
            {
                return Err_Response<::SnowK::FriendRemoveRsp>(response, rid, 
                        "Failed to delete friend session information from the database");
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

        virtual void FriendAdd(::google::protobuf::RpcController *controller,
                               const ::SnowK::FriendAddReq *request,
                               ::SnowK::FriendAddRsp *response,
                               ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string pid = request->respondent_id();

            if (_mysql_relation->Exists(uid, pid))
            {
                return Err_Response<::SnowK::FriendAddRsp>(response, rid,
                        "The two are already friends");
            }

            if (_mysql_apply->Exists(uid, pid))
            {
                return Err_Response<::SnowK::FriendAddRsp>(response, rid,
                        "Already sent a friend request to the other party");
            }

            // TODO MYSQL部分接口统一?
            std::string eid = UUID();
            FriendApply ev(eid, uid, pid);
            if (_mysql_apply->Insert(ev) == false)
            {
                return Err_Response<::SnowK::FriendAddRsp>(response, rid,
                        "Failed to add a friend event to the database");
            }

            response->set_request_id(rid);
            response->set_success(true);
            response->set_notify_event_id(eid);
        }

        virtual void FriendAddProcess(::google::protobuf::RpcController *controller,
                                      const ::SnowK::FriendAddProcessReq *request,
                                      ::SnowK::FriendAddProcessRsp *response,
                                      ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string eid = request->notify_event_id();
            std::string pid = request->apply_user_id();
            std::string uid = request->user_id();

            if (_mysql_apply->Exists(pid, uid) == false)
            {
                return Err_Response<::SnowK::FriendAddProcessRsp>(response, rid,
                        "No friend request event found");
            }

            if (_mysql_apply->Remove(pid, uid) == false)
            {
                return Err_Response<::SnowK::FriendAddProcessRsp>(response, rid,
                        "Failed to delete request event from the database");
            }

            std::string cssid;
            if (request->agree())
            {
                if (_mysql_relation->Insert(uid, pid) == false)
                {
                    return Err_Response<::SnowK::FriendAddProcessRsp>(response, rid,
                            "Failed to add friend relationship information");
                }

                cssid = UUID();
                ChatSession cs(cssid, "", ChatSessionType::SINGLE);
                if (_mysql_chat_session->Insert(cs) == false)
                {
                    return Err_Response<::SnowK::FriendAddProcessRsp>(response, rid,
                            "Failed to add one-to-one chat session information");
                }

                ChatSessionMember csm1(cssid, uid);
                ChatSessionMember csm2(cssid, pid);
                std::vector<ChatSessionMember> mlist = {csm1, csm2};
                if (_mysql_chat_session_member->Append(mlist) == false)
                {
                    return Err_Response<::SnowK::FriendAddProcessRsp>(response, rid,
                            "Inserting session member failed");
                }
            }

            response->set_request_id(rid);
            response->set_success(true);
            response->set_new_session_id(cssid);
        }

        virtual void FriendSearch(::google::protobuf::RpcController *controller,
                                  const ::SnowK::FriendSearchReq *request,
                                  ::SnowK::FriendSearchRsp *response,
                                  ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string skey = request->search_key();

            auto friend_id_lists = _mysql_relation->Friends(uid);
            friend_id_lists.push_back(uid); // Filter self out

            auto search_ret = _es_user->Search(skey, friend_id_lists);

            std::vector<std::string> user_id_lists;
            for(auto& iter : search_ret)
            {
                user_id_lists.push_back(iter.User_Id());
            }

            std::unordered_map<std::string, UserInfo> user_list;
            if (GetUserInfo(rid, user_id_lists, user_list) == false)
            {
                return Err_Response<::SnowK::FriendSearchRsp>(response, rid,
                        "Failed to obtain user information in batches");
            }

            response->set_request_id(rid);
            response->set_success(true);
            for (const auto &user_it : user_list)
            {
                auto user_info = response->add_user_info();
                user_info->CopyFrom(user_it.second);
            }
        }

        virtual void GetPendingFriendEventList(::google::protobuf::RpcController *controller,
                                               const ::SnowK::GetPendingFriendEventListReq *request,
                                               ::SnowK::GetPendingFriendEventListRsp *response,
                                               ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();

            auto ret = _mysql_apply->ApplyUsers(uid);

            std::unordered_map<std::string, UserInfo> user_list;
            if (GetUserInfo(rid, ret, user_list) == false)
            {
                return Err_Response<::SnowK::GetPendingFriendEventListRsp>(response, rid,
                        "Failed to obtain user information in batches");
            }

            response->set_request_id(rid);
            response->set_success(true);
            for (const auto &user_it : user_list)
            {
                auto ev = response->add_event();
                ev->mutable_sender()->CopyFrom(user_it.second);
            }
        }

        // Function of obtaining chat sessions: After a user successfully logs in, 
            // he can display his historical chat information
        virtual void GetChatSessionList(::google::protobuf::RpcController *controller,
                                        const ::SnowK::GetChatSessionListReq *request,
                                        ::SnowK::GetChatSessionListRsp *response,
                                        ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();

            auto sf_list = _mysql_chat_session->SingleChatSessions(uid);

            std::vector<std::string> users_id_list;
            for(const auto& f : sf_list)
            {
                users_id_list.push_back(f.friend_id);
            }

            std::unordered_map<std::string, UserInfo> user_list;
            if (GetUserInfo(rid, users_id_list, user_list) == false)
            {
                return Err_Response<::SnowK::GetChatSessionListRsp>(response, rid,
                        "Failed to get user information in bulk");
            }

            for (const auto &f : sf_list)
            {
                auto chat_session_info = response->add_chat_session_info_list();
                chat_session_info->set_single_chat_friend_id(f.friend_id);
                chat_session_info->set_chat_session_id(f.chat_session_id);
                chat_session_info->set_chat_session_name(user_list[f.friend_id].nickname());
                chat_session_info->set_avatar(user_list[f.friend_id].avatar());

                MessageInfo msg;
                if (GetRecentMsg(rid, f.chat_session_id, msg) == false)
                {
                    continue;
                }
                chat_session_info->mutable_prev_message()->CopyFrom(msg);
            }

            auto gc_list = _mysql_chat_session->GroupChatSessions(uid);
            for (const auto &f : gc_list)
            {
                auto chat_session_info = response->add_chat_session_info_list();
                chat_session_info->set_chat_session_id(f.chat_session_id);
                chat_session_info->set_chat_session_name(f.chat_session_name);

                MessageInfo msg;
                if (GetRecentMsg(rid, f.chat_session_id, msg) == false)
                {
                    continue;
                }
                chat_session_info->mutable_prev_message()->CopyFrom(msg);
            }

            response->set_request_id(rid);
            response->set_success(true);
        }

        // Creating a session, actually for users to create a group chat session
        virtual void ChatSessionCreate(::google::protobuf::RpcController *controller,
                                       const ::SnowK::ChatSessionCreateReq *request,
                                       ::SnowK::ChatSessionCreateRsp *response,
                                       ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string cssname = request->chat_session_name();

            std::string cssid = UUID();
            ChatSession cs(cssid, cssname, ChatSessionType::GROUP);
            if (_mysql_chat_session->Insert(cs) == false)
            {
                return Err_Response<::SnowK::ChatSessionCreateRsp>(response, rid,
                        "Failed to add session information to the database");
            }

            std::vector<ChatSessionMember> member_list;
            for (int i = 0; i < request->member_id_list_size(); i++)
            {
                ChatSessionMember csm(cssid, request->member_id_list(i));
                member_list.push_back(csm);
            }

            if (_mysql_chat_session_member->Append(member_list) == false)
            {
                return Err_Response<::SnowK::ChatSessionCreateRsp>(response, rid,
                        "Failed to add session member information to the database");
            }

            response->set_request_id(rid);
            response->set_success(true);
            response->mutable_chat_session_info()->set_chat_session_id(cssid);
            response->mutable_chat_session_info()->set_chat_session_name(cssname);
        }

        // Used when users view group chat member information: display member information
        virtual void GetChatSessionMember(::google::protobuf::RpcController *controller,
                                          const ::SnowK::GetChatSessionMemberReq *request,
                                          ::SnowK::GetChatSessionMemberRsp *response,
                                          ::google::protobuf::Closure *done)
        {
            brpc::ClosureGuard rpc_guard(done);

            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string cssid = request->chat_session_id();

            auto member_id_lists = _mysql_chat_session_member->Members(cssid);

            std::unordered_map<std::string, UserInfo> user_list;
            if (GetUserInfo(rid, member_id_lists, user_list) == false)
            {
                return Err_Response<::SnowK::GetChatSessionMemberRsp>(response, rid,
                        "Failed to get user information from user subservice");
            }

            response->set_request_id(rid);
            response->set_success(true);
            for (const auto &uit : user_list)
            {
                auto user_info = response->add_member_info_list();
                user_info->CopyFrom(uit.second);
            }
        }

    private:
        bool GetRecentMsg(const std::string &rid,
                          const std::string &cssid, MessageInfo &msg)
        {
            auto channel = _svrmgr_channels->Choose(_message_service_name);
            if (!channel)
            {
                LOG_ERROR("{} - There are no message sub-service nodes available for access", rid);
                return false;
            }

            brpc::Controller cntl;
            GetRecentMsgReq req;
            GetRecentMsgRsp rsp;
            req.set_request_id(rid);
            req.set_chat_session_id(cssid);
            req.set_msg_count(1);

            SnowK::MsgStorageService_Stub stub(channel.get());
            stub.GetRecentMsg(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                LOG_ERROR("{} - The message subservice call failed: {}",
                          rid, cntl.ErrorText());
                return false;
            }
            if (rsp.success() == false)
            {
                LOG_ERROR("{} - Failed to get recent messages for session {}: {}", rid, cssid, rsp.errmsg());
                return false;
            }

            if (rsp.msg_list_size() > 0)
            {
                msg.CopyFrom(rsp.msg_list(0));
                return true;
            }

            return false;
        }

        bool GetUserInfo(const std::string &rid,
                         const std::vector<std::string> &uid_list,
                         std::unordered_map<std::string, UserInfo> &user_list)
        {
            auto channel = _svrmgr_channels->Choose(_user_service_name);
            if (!channel)
            {
                LOG_ERROR("{} - There are no user sub-service nodes available for access", rid);
                return false;
            }

            brpc::Controller cntl;
            GetMultiUserInfoReq req;
            GetMultiUserInfoRsp rsp;
            req.set_request_id(rid);

            for (const auto &id : uid_list)
            {
                req.add_users_id(id);
            }

            SnowK::UserService_Stub stub(channel.get());
            stub.GetMultiUserInfo(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed())
            {
                LOG_ERROR("{} - The file subservice call failed: {}", rid, cntl.ErrorText());
                return false;
            }
            if (rsp.success() == false)
            {
                LOG_ERROR("{} - Failed to obtain user information in batches: {}", 
                          rid, rsp.errmsg());
                return false;
            }
            
            for (const auto &user_it : rsp.users_info())
            {
                user_list.insert(std::make_pair(user_it.first, user_it.second));
            }

            return true;
        }

    private:
        ESUser::ptr _es_user;

        FriendApplyTable::ptr _mysql_apply;
        ChatSessionTable::ptr _mysql_chat_session;
        ChatSessionMemeberTable::ptr _mysql_chat_session_member;
        RelationTable::ptr _mysql_relation;

        std::string _user_service_name;
        std::string _message_service_name;
        ServiceManager::ptr _svrmgr_channels;
    };

    class FriendServer
    {
    public:
        using ptr = std::shared_ptr<FriendServer>;
        FriendServer(const Discovery::ptr service_discoverer,
                     const Registry::ptr &reg_client,
                     const std::shared_ptr<brpc::Server> &server)
            : _service_discoverer(service_discoverer)
            , _registry_client(reg_client)
            , _rpc_server(server) 
        {}
        ~FriendServer() {}

        void Start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }

    private:
        Registry::ptr _registry_client;
        Discovery::ptr _service_discoverer;
        std::shared_ptr<brpc::Server> _rpc_server;
    };
    
    // Builder Pattern
    class FriendServerBuilder
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

        void Make_Discovery_Object(const std::string &reg_host,
                                   const std::string &base_service_name,
                                   const std::string &user_service_name,
                                   const std::string &message_service_name)
        {
            _user_service_name = user_service_name;
            _message_service_name = message_service_name;

            _svrmgr_channels = std::make_shared<ServiceManager>();
            _svrmgr_channels->Declare(user_service_name);
            _svrmgr_channels->Declare(message_service_name);

            LOG_DEBUG("Set the message sub-service as the sub-service to be added and managed: {}", message_service_name);
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
            FriendServiceImpl *friend_service = new FriendServiceImpl(_es_client, _mysql_client, _svrmgr_channels, 
                                                                      _user_service_name, _message_service_name);
                                                                      
            if (_rpc_server->AddService(friend_service, brpc::ServiceOwnership::SERVER_OWNS_SERVICE) == -1)
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

        FriendServer::ptr Build()
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

            FriendServer::ptr server = std::make_shared<FriendServer>(
                _service_discoverer, _registry_client, _rpc_server);

            return server;
        }

    private:
        Registry::ptr _registry_client;
        Discovery::ptr _service_discoverer;

        std::shared_ptr<elasticlient::Client> _es_client;
        std::shared_ptr<odb::core::database> _mysql_client;

        std::string _user_service_name;
        std::string _message_service_name;
        ServiceManager::ptr _svrmgr_channels;

        std::shared_ptr<brpc::Server> _rpc_server;
    }; // end of FriendServerBuilder
}