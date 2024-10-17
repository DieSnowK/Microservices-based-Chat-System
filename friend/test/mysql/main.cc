#include <gflags/gflags.h>
#include "mysql_chat_session.hpp"
#include "mysql_apply.hpp"
#include "mysql_relation.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

void R_Insert_Test(SnowK::RelationTable &tb)
{
    tb.Insert("用户ID1", "用户ID2");
    tb.Insert("用户ID1", "用户ID3");
}

void R_Select_Test(SnowK::RelationTable &tb)
{
    auto ret = tb.Friends("c4dc68239a9a0001");
    for (auto &uid : ret)
    {
        std::cout << uid << std::endl;
    }
}

void R_Remove_Test(SnowK::RelationTable &tb)
{
    tb.Remove("用户ID2", "用户ID1");
}

void R_Exists_Test(SnowK::RelationTable &tb)
{
    std::cout << tb.Exists("用户ID2", "用户ID1") << std::endl;
    std::cout << tb.Exists("用户ID3", "用户ID1") << std::endl;
}

void A_Insert_Test(SnowK::FriendApplyTable &tb)
{
    SnowK::FriendApply fa1("uuid1", "用户ID1", "用户ID2");
    tb.Insert(fa1);

    SnowK::FriendApply fa2("uuid2", "用户ID1", "用户ID3");
    tb.Insert(fa2);

    SnowK::FriendApply fa3("uuid3", "用户ID2", "用户ID3");
    tb.Insert(fa3);
}
void A_Remove_Test(SnowK::FriendApplyTable &tb)
{
    tb.Remove("用户ID2", "用户ID3");
}

void A_Select_Test(SnowK::FriendApplyTable &tb)
{
    // SnowK::FriendApply fa3("uuid3", "用户ID2", "用户ID3");
    // tb.Insert(fa3);

    auto ret = tb.ApplyUsers("用户ID2");
    for (auto &uid : ret)
    {
        std::cout << uid << std::endl;
    }
}

void A_Exists_Test(SnowK::FriendApplyTable &tb)
{
    std::cout << tb.Exists("731f50086884-0000", "c4dc68239a9a-0001") << std::endl;
    std::cout << tb.Exists("31ab86a1209d-0000", "c4dc68239a9a-0001") << std::endl;
    std::cout << tb.Exists("053f04e5e4c5-0001", "c4dc68239a9a-0001") << std::endl;
}

void C_Insert_Test(SnowK::ChatSessionTable &tb)
{
    SnowK::ChatSession cs1("会话ID1", "会话名称1", SnowK::ChatSessionType::SINGLE);
    tb.Insert(cs1);
    SnowK::ChatSession cs2("会话ID2", "会话名称2", SnowK::ChatSessionType::GROUP);
    tb.Insert(cs2);
}

// TODO
void C_Select_Test(SnowK::ChatSessionTable &tb)
{
    auto res = tb.Select("会话ID1");
    std::cout << res->Chat_Session_Id() << std::endl;
    std::cout << res->Chat_Session_Name() << std::endl;
    std::cout << (int)res->Chat_Session_Type() << std::endl;
}

void C_Single_Test(SnowK::ChatSessionTable &tb)
{
    auto ret = tb.SingleChatSessions("731f500868840000");
    for (auto &info : ret)
    {
        std::cout << info.chat_session_id << std::endl;
        std::cout << info.friend_id << std::endl;
    }
}

void C_Group_Test(SnowK::ChatSessionTable &tb)
{
    auto ret = tb.GroupChatSessions("用户ID1");
    for (auto &info : ret)
    {
        std::cout << info.chat_session_id << std::endl;
        std::cout << info.chat_session_name << std::endl;
    }
}

void C_Remove_Test(SnowK::ChatSessionTable &tb)
{
    tb.Remove("会话ID3");
}

void C_Remove_Test2(SnowK::ChatSessionTable &tb)
{
    tb.Remove("731f50086884-0000", "c4dc68239a9a-0001");
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    auto db = SnowK::ODBFactory::Create("root", "SnowK8989", "127.0.0.1", "SnowK_Test", "utf8", 0, 1);
    SnowK::RelationTable rtb(db);
    SnowK::FriendApplyTable fatb(db);
    SnowK::ChatSessionTable cstb(db);

    // R_Insert_Test(rtb);
    // R_Select_Test(rtb);
    // R_Remove_Test(rtb);
    // R_Exists_Test(rtb);

    // A_Insert_Test(fatb);
    // A_Remove_Test(fatb);
    // A_Select_Test(fatb);
    // A_Exists_Test(fatb);

    // C_Insert_Test(cstb);
    // C_Select_Test(cstb);
    // C_Single_Test(cstb);
    // std::cout << "--------------\n";
    // C_Group_Test(cstb);
    // C_Remove_Test(cstb);
    // C_Remove_Test2(cstb);

    return 0;
}