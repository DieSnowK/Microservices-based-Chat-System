#include <gflags/gflags.h>
#include "rabbitmq.hpp"
#include "../spdlog/logger.hpp"

DEFINE_string(user, "root", "RabbitMq access username");
DEFINE_string(pwd, "SnowK8989", "RabbitMQ access password");
DEFINE_string(host, "127.0.0.1:5672", "RabbitMQ server address: host:port");

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    SnowK::MQClient client(FLAGS_user, FLAGS_pwd, FLAGS_host);
    client.DeclareComponents("test-exchange", "test-queue");
    for (int i = 0; i < 5; i++)
    {
        std::string msg = "Hello SnowK-" + std::to_string(i);
        if (client.Publish("test-exchange", msg) == false)
        {
            LOG_ERROR("Failed to publis");
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}