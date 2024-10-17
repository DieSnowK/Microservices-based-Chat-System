// ʵ������ʶ���ӷ���
#include <brpc/server.h>
#include <butil/logging.h>

#include "data_es.hpp"
#include "mysql_chat_session_member.hpp"
#include "mysql_chat_session.hpp"
#include "mysql_relation.hpp"
#include "mysql_apply.hpp"
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
            , _user_service_name(user_service_name)
            , _message_service_name(message_service_name)
            , _svrmgr_channels(channel_manager) 
        {}
        ~FriendServiceImpl() {}

        virtual void GetFriendList(::google::protobuf::RpcController *controller,
                                   const ::SnowK::GetFriendListReq *request,
                                   ::SnowK::GetFriendListRsp *response,
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

            // 1. ��ȡ�����еĹؼ�Ҫ�أ��û�ID
            std::string rid = request->request_id();
            std::string uid = request->user_id();
            // 2. �����ݿ��в�ѯ��ȡ�û��ĺ���ID
            auto friend_id_lists = _mysql_relation->friends(uid);
            std::unordered_set<std::string> user_id_lists;
            for (auto &id : friend_id_lists)
            {
                user_id_lists.insert(id);
            }
            // 3. ���û��ӷ���������ȡ�û���Ϣ
            std::unordered_map<std::string, UserInfo> user_list;
            bool ret = GetUserInfo(rid, user_id_lists, user_list);
            if (ret == false)
            {
                LOG_ERROR("{} - ������ȡ�û���Ϣʧ��!", rid);
                return Err_Response(rid, "������ȡ�û���Ϣʧ��!");
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
            auto Err_Response = [this, response](const std::string &rid,
                                                 const std::string &errmsg) -> void
            {
                response->set_request_id(rid);
                response->set_success(false);
                response->set_errmsg(errmsg);
                return;
            };

            // 1. ��ȡ�ؼ�Ҫ�أ���ǰ�û�ID��Ҫɾ���ĺ���ID
            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string pid = request->peer_id();
            // 2. �Ӻ��ѹ�ϵ����ɾ�����ѹ�ϵ��Ϣ
            bool ret = _mysql_relation->remove(uid, pid);
            if (ret == false)
            {
                LOG_ERROR("{} - �����ݿ�ɾ��������Ϣʧ�ܣ�", rid);
                return Err_Response(rid, "�����ݿ�ɾ��������Ϣʧ�ܣ�");
            }
            // 3. �ӻỰ��Ϣ���У�ɾ����Ӧ������Ự -- ͬʱɾ���Ự��Ա���еĳ�Ա��Ϣ
            ret = _mysql_chat_session->remove(uid, pid);
            if (ret == false)
            {
                LOG_ERROR("{}- �����ݿ�ɾ�����ѻỰ��Ϣʧ�ܣ�", rid);
                return Err_Response(rid, "�����ݿ�ɾ�����ѻỰ��Ϣʧ�ܣ�");
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
            auto Err_Response = [this, response](const std::string &rid,
                                                 const std::string &errmsg) -> void
            {
                response->set_request_id(rid);
                response->set_success(false);
                response->set_errmsg(errmsg);
                return;
            };

            // 1. ��ȡ�����еĹؼ�Ҫ�أ��������û�ID�� ���������û�ID
            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string pid = request->respondent_id();
            // 2. �ж������Ƿ��Ѿ��Ǻ���
            bool ret = _mysql_relation->exists(uid, pid);
            if (ret == true)
            {
                LOG_ERROR("{}- �������ʧ��-����{}-{}�Ѿ��Ǻ��ѹ�ϵ", rid, uid, pid);
                return Err_Response(rid, "�����Ѿ��Ǻ��ѹ�ϵ��");
            }
            // 3. ��ǰ�Ƿ��Ѿ����������
            ret = _mysql_apply->exists(uid, pid);
            if (ret == true)
            {
                LOG_ERROR("{}- �������ʧ��-�Ѿ�������Է����ѣ�", rid, uid, pid);
                return Err_Response(rid, "�Ѿ�������Է����ѣ�");
            }
            // 4. �����������У�����������Ϣ
            std::string eid = uuid();
            FriendApply ev(eid, uid, pid);
            ret = _mysql_apply->insert(ev);
            if (ret == false)
            {
                LOG_ERROR("{} - �����ݿ��������������¼�ʧ�ܣ�", rid);
                return Err_Response(rid, "�����ݿ��������������¼�ʧ�ܣ�");
            }
            // 3. ��֯��Ӧ
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
            auto Err_Response = [this, response](const std::string &rid,
                                                 const std::string &errmsg) -> void
            {
                response->set_request_id(rid);
                response->set_success(false);
                response->set_errmsg(errmsg);
                return;
            };

            // 1. ��ȡ�����еĹؼ�Ҫ�أ��������û�ID�����������û�ID�����������¼�ID
            std::string rid = request->request_id();
            std::string eid = request->notify_event_id();
            std::string uid = request->user_id();       // ��������
            std::string pid = request->apply_user_id(); // ������
            bool agree = request->agree();
            // 2. �ж���û�и������¼�
            bool ret = _mysql_apply->exists(pid, uid);
            if (ret == false)
            {
                LOG_ERROR("{}- û���ҵ�{}-{}��Ӧ�ĺ��������¼���", rid, pid, uid);
                return Err_Response(rid, "û���ҵ���Ӧ�ĺ��������¼�!");
            }
            // 3. ����У� ���Դ��� --- ɾ�������¼�--�¼��Ѿ��������
            ret = _mysql_apply->remove(pid, uid);
            if (ret == false)
            {
                LOG_ERROR("{}- �����ݿ�ɾ�������¼� {}-{} ʧ�ܣ�", rid, pid, uid);
                return Err_Response(rid, "�����ݿ�ɾ�������¼�ʧ��!");
            }
            // 4. �����������ͬ�⣺�����ݿ��������ѹ�ϵ��Ϣ���������ĻỰ��Ϣ���Ự��Ա
            std::string cssid;
            if (agree == true)
            {
                ret = _mysql_relation->insert(uid, pid);
                if (ret == false)
                {
                    LOG_ERROR("{}- �������ѹ�ϵ��Ϣ-{}-{}��", rid, uid, pid);
                    return Err_Response(rid, "�������ѹ�ϵ��Ϣ!");
                }
                cssid = uuid();
                ChatSession cs(cssid, "", ChatSessionType::SINGLE);
                ret = _mysql_chat_session->insert(cs);
                if (ret == false)
                {
                    LOG_ERROR("{}- �������ĻỰ��Ϣ-{}��", rid, cssid);
                    return Err_Response(rid, "�������ĻỰ��Ϣʧ��!");
                }
                ChatSessionMember csm1(cssid, uid);
                ChatSessionMember csm2(cssid, pid);
                std::vector<ChatSessionMember> mlist = {csm1, csm2};
                ret = _mysql_chat_session_member->append(mlist);
                if (ret == false)
                {
                    LOG_ERROR("{}- û���ҵ�{}-{}��Ӧ�ĺ��������¼���", rid, pid, uid);
                    return Err_Response(rid, "û���ҵ���Ӧ�ĺ��������¼�!");
                }
            }
            // 5. ��֯��Ӧ
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
            auto Err_Response = [this, response](const std::string &rid,
                                                 const std::string &errmsg) -> void
            {
                response->set_request_id(rid);
                response->set_success(false);
                response->set_errmsg(errmsg);
                return;
            };

            // 1. ��ȡ�����еĹؼ�Ҫ�أ������ؼ��֣��������û�ID���������ֻ��ţ��������ǳƵ�һ���֣�
            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string skey = request->search_key();
            LOG_DEBUG("{} �������� �� {}", uid, skey);
            // 2. �����û�ID����ȡ�û��ĺ���ID�б�
            auto friend_id_lists = _mysql_relation->friends(uid);
            // 3. ��ES������������û���Ϣ���� --- ���˵���ǰ�ĺ���
            std::unordered_set<std::string> user_id_lists;
            friend_id_lists.push_back(uid); // ���Լ�Ҳ���˵�
            auto search_res = _es_user->search(skey, friend_id_lists);
            for (auto &it : search_res)
            {
                user_id_lists.insert(it.user_id());
            }
            // 4. ���ݻ�ȡ�����û�ID�� ���û��ӷ��������������û���Ϣ��ȡ
            std::unordered_map<std::string, UserInfo> user_list;
            bool ret = GetUserInfo(rid, user_id_lists, user_list);
            if (ret == false)
            {
                LOG_ERROR("{} - ������ȡ�û���Ϣʧ��!", rid);
                return Err_Response(rid, "������ȡ�û���Ϣʧ��!");
            }
            // 5. ��֯��Ӧ
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
            auto Err_Response = [this, response](const std::string &rid,
                                                 const std::string &errmsg) -> void
            {
                response->set_request_id(rid);
                response->set_success(false);
                response->set_errmsg(errmsg);
                return;
            };

            // 1. ��ȡ�ؼ�Ҫ�أ���ǰ�û�ID
            std::string rid = request->request_id();
            std::string uid = request->user_id();
            // 2. �����ݿ��ȡ������������¼���Ϣ --- �������û�ID�б�
            auto res = _mysql_apply->applyUsers(uid);
            std::unordered_set<std::string> user_id_lists;
            for (auto &id : res)
            {
                user_id_lists.insert(id);
            }
            // 3. ������ȡ�������û���Ϣ��
            std::unordered_map<std::string, UserInfo> user_list;
            bool ret = GetUserInfo(rid, user_id_lists, user_list);
            if (ret == false)
            {
                LOG_ERROR("{} - ������ȡ�û���Ϣʧ��!", rid);
                return Err_Response(rid, "������ȡ�û���Ϣʧ��!");
            }
            // 4. ��֯��Ӧ
            response->set_request_id(rid);
            response->set_success(true);
            for (const auto &user_it : user_list)
            {
                auto ev = response->add_event();
                ev->mutable_sender()->CopyFrom(user_it.second);
            }
        }

        virtual void GetChatSessionList(::google::protobuf::RpcController *controller,
                                        const ::SnowK::GetChatSessionListReq *request,
                                        ::SnowK::GetChatSessionListRsp *response,
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

            // ��ȡ����Ự�����ã�һ���û���¼�ɹ����ܹ�չʾ�Լ�����ʷ������Ϣ
            // 1. ��ȡ�����еĹؼ�Ҫ�أ���ǰ�����û�ID
            std::string rid = request->request_id();
            std::string uid = request->user_id();
            // 2. �����ݿ��в�ѯ���û��ĵ��ĻỰ�б�
            auto sf_list = _mysql_chat_session->singleChatSession(uid);
            //  1. �ӵ��ĻỰ�б��У�ȡ�����еĺ���ID�����û��ӷ����ȡ�û���Ϣ
            std::unordered_set<std::string> users_id_list;
            for (const auto &f : sf_list)
            {
                users_id_list.insert(f.friend_id);
            }
            std::unordered_map<std::string, UserInfo> user_list;
            bool ret = GetUserInfo(rid, users_id_list, user_list);
            if (ret == false)
            {
                LOG_ERROR("{} - ������ȡ�û���Ϣʧ�ܣ�", rid);
                return Err_Response(rid, "������ȡ�û���Ϣʧ��!");
            }
            //  2. ������Ӧ�Ự��Ϣ���Ự���ƾ��Ǻ������ƣ��Ựͷ����Ǻ���ͷ��
            // 3. �����ݿ��в�ѯ���û���Ⱥ�ĻỰ�б�
            auto gc_list = _mysql_chat_session->groupChatSession(uid);

            // 4. �������еĻỰID������Ϣ�洢�ӷ����ȡ�Ự���һ����Ϣ
            // 5. ��֯��Ӧ
            for (const auto &f : sf_list)
            {
                auto chat_session_info = response->add_chat_session_info_list();
                chat_session_info->set_single_chat_friend_id(f.friend_id);
                chat_session_info->set_chat_session_id(f.chat_session_id);
                chat_session_info->set_chat_session_name(user_list[f.friend_id].nickname());
                chat_session_info->set_avatar(user_list[f.friend_id].avatar());
                MessageInfo msg;
                ret = GetRecentMsg(rid, f.chat_session_id, msg);
                if (ret == false)
                {
                    continue;
                }
                chat_session_info->mutable_prev_message()->CopyFrom(msg);
            }
            for (const auto &f : gc_list)
            {
                auto chat_session_info = response->add_chat_session_info_list();
                chat_session_info->set_chat_session_id(f.chat_session_id);
                chat_session_info->set_chat_session_name(f.chat_session_name);
                MessageInfo msg;
                ret = GetRecentMsg(rid, f.chat_session_id, msg);
                if (ret == false)
                {
                    continue;
                }
                chat_session_info->mutable_prev_message()->CopyFrom(msg);
            }
            response->set_request_id(rid);
            response->set_success(true);
        }

        virtual void ChatSessionCreate(::google::protobuf::RpcController *controller,
                                       const ::SnowK::ChatSessionCreateReq *request,
                                       ::SnowK::ChatSessionCreateRsp *response,
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

            // �����Ự����ʵ��Ե����û�Ҫ����һ��Ⱥ�ĻỰ
            // 1. ��ȡ����ؼ�Ҫ�أ��Ự���ƣ��Ự��Ա
            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string cssname = request->chat_session_name();

            // 2. ���ɻỰID�������ݿ���ӻỰ��Ϣ����ӻỰ��Ա��Ϣ
            std::string cssid = uuid();
            ChatSession cs(cssid, cssname, ChatSessionType::GROUP);
            bool ret = _mysql_chat_session->insert(cs);
            if (ret == false)
            {
                LOG_ERROR("{} - �����ݿ���ӻỰ��Ϣʧ��: {}", rid, cssname);
                return Err_Response(rid, "�����ݿ���ӻỰ��Ϣʧ��!");
            }
            std::vector<ChatSessionMember> member_list;
            for (int i = 0; i < request->member_id_list_size(); i++)
            {
                ChatSessionMember csm(cssid, request->member_id_list(i));
                member_list.push_back(csm);
            }
            ret = _mysql_chat_session_member->append(member_list);
            if (ret == false)
            {
                LOG_ERROR("{} - �����ݿ���ӻỰ��Ա��Ϣʧ��: {}", rid, cssname);
                return Err_Response(rid, "�����ݿ���ӻỰ��Ա��Ϣʧ��!");
            }
            // 3. ��֯��Ӧ---��֯�Ự��Ϣ
            response->set_request_id(rid);
            response->set_success(true);
            response->mutable_chat_session_info()->set_chat_session_id(cssid);
            response->mutable_chat_session_info()->set_chat_session_name(cssname);
        }

        virtual void GetChatSessionMember(::google::protobuf::RpcController *controller,
                                          const ::SnowK::GetChatSessionMemberReq *request,
                                          ::SnowK::GetChatSessionMemberRsp *response,
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

            // �����û��鿴Ⱥ�ĳ�Ա��Ϣ��ʱ�򣺽��г�Ա��Ϣչʾ
            // 1. ��ȡ�ؼ�Ҫ�أ�����ỰID
            std::string rid = request->request_id();
            std::string uid = request->user_id();
            std::string cssid = request->chat_session_id();
            // 2. �����ݿ��ȡ�Ự��ԱID�б�
            auto member_id_lists = _mysql_chat_session_member->members(cssid);
            std::unordered_set<std::string> uid_list;
            for (const auto &id : member_id_lists)
            {
                uid_list.insert(id);
            }
            // 3. ���û��ӷ���������ȡ�û���Ϣ
            std::unordered_map<std::string, UserInfo> user_list;
            bool ret = GetUserInfo(rid, uid_list, user_list);
            if (ret == false)
            {
                LOG_ERROR("{} - ���û��ӷ����ȡ�û���Ϣʧ�ܣ�", rid);
                return Err_Response(rid, "���û��ӷ����ȡ�û���Ϣʧ��!");
            }
            // 4. ��֯��Ӧ
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
            auto channel = _svrmgr_channels->choose(_message_service_name);
            if (!channel)
            {
                LOG_ERROR("{} - ��ȡ��Ϣ�ӷ����ŵ�ʧ�ܣ���", rid);
                return false;
            }
            GetRecentMsgReq req;
            GetRecentMsgRsp rsp;
            req.set_request_id(rid);
            req.set_chat_session_id(cssid);
            req.set_msg_count(1);
            brpc::Controller cntl;
            SnowK::MsgStorageService_Stub stub(channel.get());
            stub.GetRecentMsg(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed() == true)
            {
                LOG_ERROR("{} - ��Ϣ�洢�ӷ������ʧ��: {}", rid, cntl.ErrorText());
                return false;
            }
            if (rsp.success() == false)
            {
                LOG_ERROR("{} - ��ȡ�Ự {} �����Ϣʧ��: {}", rid, cssid, rsp.errmsg());
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
                         const std::unordered_set<std::string> &uid_list,
                         std::unordered_map<std::string, UserInfo> &user_list)
        {
            auto channel = _svrmgr_channels->choose(_user_service_name);
            if (!channel)
            {
                LOG_ERROR("{} - ��ȡ�û��ӷ����ŵ�ʧ�ܣ���", rid);
                return false;
            }
            GetMultiUserInfoReq req;
            GetMultiUserInfoRsp rsp;
            req.set_request_id(rid);
            for (auto &id : uid_list)
            {
                req.add_users_id(id);
            }
            brpc::Controller cntl;
            SnowK::UserService_Stub stub(channel.get());
            stub.GetMultiUserInfo(&cntl, &req, &rsp, nullptr);
            if (cntl.Failed() == true)
            {
                LOG_ERROR("{} - �û��ӷ������ʧ��: {}", rid, cntl.ErrorText());
                return false;
            }
            if (rsp.success() == false)
            {
                LOG_ERROR("{} - ������ȡ�û���Ϣʧ��: {}", rid, rsp.errmsg());
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
                     const std::shared_ptr<elasticlient::Client> &es_client,
                     const std::shared_ptr<odb::core::database> &mysql_client,
                     const std::shared_ptr<brpc::Server> &server) 
            : _service_discoverer(service_discoverer)
            , _registry_client(reg_client)
            , _es_client(es_client)
            , _mysql_client(mysql_client)
            , _rpc_server(server) 
        {}
        ~FriendServer() {}

        void Start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }

    private:
        Discovery::ptr _service_discoverer;
        Registry::ptr _registry_client;
        std::shared_ptr<elasticlient::Client> _es_client;
        std::shared_ptr<odb::core::database> _mysql_client;
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
            _svrmgr_channels->Declare(message_service_name);
            _svrmgr_channels->Declare(user_service_name);

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
                _service_discoverer, _registry_client,
                _es_client, _mysql_client, _rpc_server);

            return server;
        }

    private:
        Registry::ptr _registry_client;

        std::shared_ptr<elasticlient::Client> _es_client;
        std::shared_ptr<odb::core::database> _mysql_client;

        std::string _user_service_name;
        std::string _message_service_name;
        ServiceManager::ptr _svrmgr_channels;
        Discovery::ptr _service_discoverer;

        std::shared_ptr<brpc::Server> _rpc_server;
    }; // end of FriendServerBuilder
}