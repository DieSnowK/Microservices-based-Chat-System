#include <gflags/gflags.h>
#include "mysql_message.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

void Insert_Test(SnowK::MessageTable &tb)
{
    SnowK::Message m1("消息ID1", "会话ID1", "用户ID1", 0, boost::posix_time::time_from_string("2002-01-20 23:59:59.000"));
    tb.Insert(m1);
    SnowK::Message m2("消息ID2", "会话ID1", "用户ID2", 0, boost::posix_time::time_from_string("2002-01-21 23:59:59.000"));
    tb.Insert(m2);
    SnowK::Message m3("消息ID3", "会话ID1", "用户ID3", 0, boost::posix_time::time_from_string("2002-01-22 23:59:59.000"));
    tb.Insert(m3);

    SnowK::Message m4("消息ID4", "会话ID2", "用户ID4", 0, boost::posix_time::time_from_string("2002-01-20 23:59:59.000"));
    tb.Insert(m4);
    SnowK::Message m5("消息ID5", "会话ID2", "用户ID5", 0, boost::posix_time::time_from_string("2002-01-21 23:59:59.000"));
    tb.Insert(m5);
}

void Remove_Test(SnowK::MessageTable &tb)
{
    tb.Remove("会话ID2");
}

void Recent_Test(SnowK::MessageTable &tb)
{
    auto ret = tb.Recent("会话ID1", 2);
    for (auto begin = ret.begin(); begin != ret.end(); ++begin)
    {
        std::cout << begin->Message_Id() << std::endl;
        std::cout << begin->Session_Id() << std::endl;
        std::cout << begin->User_Id() << std::endl;
        std::cout << boost::posix_time::to_simple_string(begin->Create_Time()) << std::endl;
    }
}

void Range_Test(SnowK::MessageTable &tb)
{
    boost::posix_time::ptime stime(boost::posix_time::time_from_string("2002-01-20 23:59:59.000"));
    boost::posix_time::ptime etime(boost::posix_time::time_from_string("2002-01-21 23:59:59.000"));

    auto ret = tb.Range("会话ID1", stime, etime);
    for (auto begin = ret.begin(); begin != ret.end(); ++begin)
    {
        std::cout << begin->Message_Id() << std::endl;
        std::cout << begin->Session_Id() << std::endl;
        std::cout << begin->User_Id() << std::endl;
        std::cout << boost::posix_time::to_simple_string(begin->Create_Time()) << std::endl;
    }
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    auto db = SnowK::ODBFactory::Create("root", "SnowK8989", "127.0.0.1", "SnowK_Test", "utf8", 0, 1);
    SnowK::MessageTable tb(db);

    // Insert_Test(tb);
    // Remove_Test(tb);
    // Recent_Test(tb);
    // std::cout << "-------------------------" << std::endl;
    // Range_Test(tb);

    return 0;
}