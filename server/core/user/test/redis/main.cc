#include <gflags/gflags.h>
#include <thread>
#include "redis.hpp"
#include "data_redis.hpp"

DEFINE_string(ip, "127.0.0.1", "Server ip, format: 127.0.0.1");
DEFINE_int32(port, 6379, "Server port, format: 6379");
DEFINE_int32(db, 0, "The number of the library, default number 0");
DEFINE_bool(keep_alive, true, "Whether to keep the connection alive for a long time");

void Session_Test(const std::shared_ptr<sw::redis::Redis> &client)
{
    SnowK::Session ss(client);
    ss.Append("会话ID1", "用户ID1");
    ss.Append("会话ID2", "用户ID2");
    ss.Append("会话ID3", "用户ID3");
    ss.Append("会话ID4", "用户ID4");

    ss.Remove("会话ID2");
    ss.Remove("会话ID3");

    auto res1 = ss.Uid("会话ID1");
    if (res1)
        std::cout << *res1 << std::endl;
    auto res2 = ss.Uid("会话ID2");
    if (res2)
        std::cout << *res2 << std::endl;
    auto res3 = ss.Uid("会话ID3");
    if (res3)
        std::cout << *res3 << std::endl;
    auto res4 = ss.Uid("会话ID4");
    if (res4)
        std::cout << *res4 << std::endl;
}

void Status_Test(const std::shared_ptr<sw::redis::Redis> &client)
{
    SnowK::Status status(client);
    status.Append("用户ID1");
    status.Append("用户ID2");
    status.Append("用户ID3");

    status.Remove("用户ID2");

    if (status.Exists("用户ID1"))
        std::cout << "用户1在线" << std::endl;
    if (status.Exists("用户ID2"))
        std::cout << "用户2在线" << std::endl;
    if (status.Exists("用户ID3"))
        std::cout << "用户3在线" << std::endl;
}

void Code_Test(const std::shared_ptr<sw::redis::Redis> &client)
{
    SnowK::Codes Codes(client);
    Codes.Append("验证码ID1", "验证码1", std::chrono::milliseconds(3000));
    Codes.Append("验证码ID2", "验证码2", std::chrono::milliseconds(3000));
    Codes.Append("验证码ID3", "验证码3", std::chrono::milliseconds(3000));

    Codes.Remove("验证码ID2");

    auto y1 = Codes.Code("验证码ID1");
    auto y2 = Codes.Code("验证码ID2");
    auto y3 = Codes.Code("验证码ID3");
    if (y1)
        std::cout << *y1 << std::endl;
    if (y2)
        std::cout << *y2 << std::endl;
    if (y3)
        std::cout << *y3 << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(4));
    auto y4 = Codes.Code("验证码ID1");
    auto y5 = Codes.Code("验证码ID2");
    auto y6 = Codes.Code("验证码ID3");
    if (!y4)
        std::cout << "验证码ID1不存在" << std::endl;
    if (!y5)
        std::cout << "验证码ID2不存在" << std::endl;
    if (!y6)
        std::cout << "验证码ID3不存在" << std::endl;
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    auto client = SnowK::RedisClientFactory::Create(FLAGS_ip, FLAGS_port, FLAGS_db, FLAGS_keep_alive);

    // Session_Test(client);
    // Status_Test(client);
    Code_Test(client);

    return 0;
}
