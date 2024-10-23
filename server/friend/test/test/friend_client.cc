#include <thread>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include "etcd.hpp"
#include "channel.hpp"
#include "utils.hpp"
#include "friend.pb.h"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(friend_service, "/service/friend_service", "The root directory of service monitoring");

SnowK::ServiceManager::ptr sm;

void Apply_Test(const std::string &uid1, const std::string &uid2)
{
    auto channel = sm->Choose(FLAGS_friend_service);
    SnowK::FriendService_Stub stub(channel.get());

    brpc::Controller cntl;
    SnowK::FriendAddReq req;
    SnowK::FriendAddRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid1);
    req.set_respondent_id(uid2);
    stub.FriendAdd(&cntl, &req, &rsp, nullptr);

    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());
}

void Get_Apply_List(const std::string &uid1)
{
    auto channel = sm->Choose(FLAGS_friend_service);
    SnowK::FriendService_Stub stub(channel.get());

    brpc::Controller cntl;
    SnowK::GetPendingFriendEventListReq req;
    SnowK::GetPendingFriendEventListRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid1);
    stub.GetPendingFriendEventList(&cntl, &req, &rsp, nullptr);

    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());

    for (int i = 0; i < rsp.event_size(); i++)
    {
        std::cout << "---------------\n";
        std::cout << rsp.event(i).sender().user_id() << std::endl;
        std::cout << rsp.event(i).sender().nickname() << std::endl;
        std::cout << rsp.event(i).sender().avatar() << std::endl;
    }
}

void Process_Apply_Test(const std::string &uid, bool agree, const std::string &apply_user_id)
{
    auto channel = sm->Choose(FLAGS_friend_service);
    SnowK::FriendService_Stub stub(channel.get());

    brpc::Controller cntl;
    SnowK::FriendAddProcessReq req;
    SnowK::FriendAddProcessRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid);
    req.set_agree(agree);
    req.set_apply_user_id(apply_user_id);
    stub.FriendAddProcess(&cntl, &req, &rsp, nullptr);

    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());

    if (agree)
    {
        std::cout << rsp.new_session_id() << std::endl;
    }
}

void Search_Test(const std::string &uid, const std::string &key)
{
    auto channel = sm->Choose(FLAGS_friend_service);
    SnowK::FriendService_Stub stub(channel.get());
    
    brpc::Controller cntl;
    SnowK::FriendSearchReq req;
    SnowK::FriendSearchRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid);
    req.set_search_key(key);
    stub.FriendSearch(&cntl, &req, &rsp, nullptr);

    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());

    for (int i = 0; i < rsp.user_info_size(); i++)
    {
        std::cout << "-------------------\n";
        std::cout << rsp.user_info(i).user_id() << std::endl;
        std::cout << rsp.user_info(i).nickname() << std::endl;
        std::cout << rsp.user_info(i).avatar() << std::endl;
    }
}

void Friend_List_Test(const std::string &uid1)
{
    auto channel = sm->Choose(FLAGS_friend_service);
    SnowK::FriendService_Stub stub(channel.get());

    brpc::Controller cntl;
    SnowK::GetFriendListReq req;
    SnowK::GetFriendListRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid1);
    stub.GetFriendList(&cntl, &req, &rsp, nullptr);

    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());

    for (int i = 0; i < rsp.friend_list_size(); i++)
    {
        std::cout << "-------------------\n";
        std::cout << rsp.friend_list(i).user_id() << std::endl;
        std::cout << rsp.friend_list(i).nickname() << std::endl;
        std::cout << rsp.friend_list(i).avatar() << std::endl;
    }
}

void Remove_Test(const std::string &uid1, const std::string &uid2)
{
    auto channel = sm->Choose(FLAGS_friend_service);
    SnowK::FriendService_Stub stub(channel.get());

    brpc::Controller cntl;
    SnowK::FriendRemoveReq req;
    SnowK::FriendRemoveRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid1);
    req.set_peer_id(uid2);
    stub.FriendRemove(&cntl, &req, &rsp, nullptr);

    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());
}

void Create_Css_Test(const std::string &uid1, const std::vector<std::string> &uidlist)
{
    auto channel = sm->Choose(FLAGS_friend_service);
    SnowK::FriendService_Stub stub(channel.get());

    brpc::Controller cntl;
    SnowK::ChatSessionCreateReq req;
    SnowK::ChatSessionCreateRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid1);
    req.set_chat_session_name("We Are Family");
    for (auto &id : uidlist)
    {
        req.add_member_id_list(id);
    }

    stub.ChatSessionCreate(&cntl, &req, &rsp, nullptr);

    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());

    std::cout << rsp.chat_session_info().chat_session_id() << std::endl;
    std::cout << rsp.chat_session_info().chat_session_name() << std::endl;
}

void Cssmember_Test(const std::string &uid1, const std::string &cssid)
{
    auto channel = sm->Choose(FLAGS_friend_service);
    SnowK::FriendService_Stub stub(channel.get());

    brpc::Controller cntl;
    SnowK::GetChatSessionMemberReq req;
    SnowK::GetChatSessionMemberRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid1);
    req.set_chat_session_id(cssid);
    stub.GetChatSessionMember(&cntl, &req, &rsp, nullptr);

    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());

    for (int i = 0; i < rsp.member_info_list_size(); i++)
    {
        std::cout << "-------------------\n";
        std::cout << rsp.member_info_list(i).user_id() << std::endl;
        std::cout << rsp.member_info_list(i).nickname() << std::endl;
        std::cout << rsp.member_info_list(i).avatar() << std::endl;
    }
}

void Csslist_Test(const std::string &uid1)
{
    auto channel = sm->Choose(FLAGS_friend_service);
    SnowK::FriendService_Stub stub(channel.get());

    brpc::Controller cntl;
    SnowK::GetChatSessionListReq req;
    SnowK::GetChatSessionListRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid1);

    stub.GetChatSessionList(&cntl, &req, &rsp, nullptr);

    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());

    for (int i = 0; i < rsp.chat_session_info_list_size(); i++)
    {
        std::cout << "-------------------\n";
        std::cout << rsp.chat_session_info_list(i).single_chat_friend_id() << std::endl;
        std::cout << rsp.chat_session_info_list(i).chat_session_id() << std::endl;
        std::cout << rsp.chat_session_info_list(i).chat_session_name() << std::endl;
        std::cout << rsp.chat_session_info_list(i).avatar() << std::endl;
        std::cout << "消息内容：\n";
        std::cout << rsp.chat_session_info_list(i).prev_message().message_id() << std::endl;
        std::cout << rsp.chat_session_info_list(i).prev_message().chat_session_id() << std::endl;
        std::cout << rsp.chat_session_info_list(i).prev_message().timestamp() << std::endl;
        std::cout << rsp.chat_session_info_list(i).prev_message().sender().user_id() << std::endl;
        std::cout << rsp.chat_session_info_list(i).prev_message().sender().nickname() << std::endl;
        std::cout << rsp.chat_session_info_list(i).prev_message().sender().avatar() << std::endl;
        std::cout << rsp.chat_session_info_list(i).prev_message().message().file_message().file_name() << std::endl;
        std::cout << rsp.chat_session_info_list(i).prev_message().message().file_message().file_contents() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    sm = std::make_shared<SnowK::ServiceManager>();
    sm->Declare(FLAGS_friend_service);

    auto put_cb = std::bind(&SnowK::ServiceManager::ServiceOnline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&SnowK::ServiceManager::ServiceOffline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    SnowK::Discovery::ptr dclient = std::make_shared<SnowK::Discovery>(FLAGS_etcd_host, FLAGS_base_service, put_cb, del_cb);

    // Apply_Test("846e6111b5690000", "2e9235b101050003");
    // Apply_Test("dd40103d33540001", "2e9235b101050003");
    // Apply_Test("6d941c5ee3ef0002", "2e9235b101050003");

    // Get_Apply_List("2e9235b101050003");

    // Process_Apply_Test("2e9235b101050003", true, "846e6111b5690000");
    // Process_Apply_Test("2e9235b101050003", false, "dd40103d33540001");
    // Process_Apply_Test("2e9235b101050003", true, "6d941c5ee3ef0002");

    // std::cout << "**********************\n";
    // Search_Test("2e9235b101050003", "猪");
    // std::cout << "++++++++++++++++++++++\n";
    // Search_Test("6d941c5ee3ef0002", "猪");
    // std::cout << "======================\n";
    // Search_Test("846e6111b5690000", "猪");

    // Friend_List_Test("2e9235b101050003");
    // std::cout << "++++++++++++++++++++++\n";
    // Friend_List_Test("6d941c5ee3ef0002");
    // std::cout << "++++++++++++++++++++++\n";
    // Friend_List_Test("846e6111b5690000");

    // Remove_Test("2e9235b101050003", "846e6111b5690000");

    // std::vector<std::string> uidlist = {
    //     "846e6111b5690000",
    //     "dd40103d33540001",
    //     "6d941c5ee3ef0002",
    //     "2e9235b101050003"
    // };
    // Create_Css_Test("6d941c5ee3ef0002", uidlist);

    // Cssmember_Test("6d941c5ee3ef0002", "25f2a37515f20005");
    // std::cout << "++++++++++++++++++++++\n";
    // Cssmember_Test("2e9235b101050003", "25f2a37515f20005");

    Csslist_Test("2e9235b101050003");

    return 0;
}