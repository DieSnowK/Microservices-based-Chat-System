#include <gflags/gflags.h>
#include "mysql_chat_session_member.hpp"
#include "chat_session_member.hpp"
#include "chat_session_member-odb.hxx"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

void Append_Test(SnowK::ChatSessionMemeberTable &tb)
{
    SnowK::ChatSessionMember csm1("会话ID1", "用户ID1");
    tb.Append(csm1);
    SnowK::ChatSessionMember csm2("会话ID1", "用户ID2");
    tb.Append(csm2);
    SnowK::ChatSessionMember csm3("会话ID2", "用户ID3");
    tb.Append(csm3);
}

void Multi_Append_Test(SnowK::ChatSessionMemeberTable &tb)
{
    SnowK::ChatSessionMember csm1("会话ID3", "用户ID1");
    SnowK::ChatSessionMember csm2("会话ID3", "用户ID2");
    SnowK::ChatSessionMember csm3("会话ID3", "用户ID3");
    std::vector<SnowK::ChatSessionMember> list = {csm1, csm2, csm3};
    tb.Append(list);
}

void Remove_Test(SnowK::ChatSessionMemeberTable &tb)
{
    SnowK::ChatSessionMember csm3("会话ID2", "用户ID3");
    tb.Remove(csm3);
}

void SS_Members(SnowK::ChatSessionMemeberTable &tb)
{
    auto res = tb.Members("会话ID1");
    for (auto &id : res)
    {
        std::cout << id << std::endl;
    }
}

void Remove_All(SnowK::ChatSessionMemeberTable &tb)
{
    tb.Remove("会话ID3");
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    auto db = SnowK::ODBFactory::Create("root", "SnowK8989", "127.0.0.1", "SnowK_Test", "utf8", 0, 1);
    SnowK::ChatSessionMemeberTable csmt(db);

    // Append_Test(csmt);
    // Multi_Append_Test(csmt);
    // Remove_Test(csmt);
    // SS_Members(csmt);
    // Remove_All(csmt);

    return 0;
}