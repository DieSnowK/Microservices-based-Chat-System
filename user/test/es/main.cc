#include <thread>
#include <gflags/gflags.h>
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

    auto es_user = std::make_shared<SnowK::ESUser>(es_client);
    es_user->CreateIndex();
    es_user->appendData("用户ID1", "手机号1", "DieK", "嗨嗨嗨", "头像1");
    es_user->appendData("用户ID2", "手机号2", "DieKSnowK", "嗨嗨嗨嗨嗨嗨", "头像2");

    auto ret = es_user->search("DieK", {"用户ID1"});
    for (auto &u : ret)
    {
        std::cout << "-----------------" << std::endl;
        std::cout << u.user_id() << std::endl;
        std::cout << *u.phone() << std::endl;
        std::cout << *u.nickname() << std::endl;
        std::cout << *u.description() << std::endl;
        std::cout << *u.avatar_id() << std::endl;
    }

    return 0;
}