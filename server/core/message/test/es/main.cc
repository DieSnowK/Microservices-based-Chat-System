#include <gflags/gflags.h>
#include "es.hpp"
#include "data_es.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(es_host, "http://127.0.0.1:9200/", "ES server url");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    auto es_client = SnowK::ESClientFactory::Create({FLAGS_es_host});

    auto es_msg = std::make_shared<SnowK::ESMessage>(es_client);
    es_msg->CreateIndex();

    // es_msg->AppendData("用户ID1", "消息ID1", 1723025035, "会话ID1", "吃饭了吗？");
    // es_msg->AppendData("用户ID2", "消息ID2", 1723025035 - 100, "会话ID1", "吃的盖浇饭！");
    // es_msg->AppendData("用户ID3", "消息ID3", 1723025035, "会话ID2", "吃饭了吗？");
    // es_msg->AppendData("用户ID4", "消息ID4", 1723025035 - 100, "会话ID2", "吃的盖浇饭！");

    auto ret = es_msg->Search("盖浇饭", "会话ID1");
    for (auto &u : ret)
    {
        std::cout << "-----------------" << std::endl;
        std::cout << u.User_Id() << std::endl;
        std::cout << u.Message_Id() << std::endl;
        std::cout << u.Session_Id() << std::endl;
        std::cout << boost::posix_time::to_simple_string(u.Create_Time()) << std::endl;
        std::cout << u.Content() << std::endl;
    }

    return 0;
}