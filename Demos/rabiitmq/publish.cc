#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

int main()
{
    // 1.实例化底层网络通信框架的IO事件监控句柄
    auto *loop = EV_DEFAULT;

    // 2.实例化libEvHandler句柄 -> 将AMQP框架与事件监控关联起来
    AMQP::LibEvHandler handler(loop);

    // 3.实例化连接对象
    AMQP::Address address("amqp://root:SnowK8989@127.0.0.1:5672/");
    AMQP::TcpConnection connection(&handler, address);

    // 4.实例化信道对象
    AMQP::TcpChannel channel(&connection);

    // 5.声明交换机
    channel.declareExchange("test-exchange", AMQP::ExchangeType::direct)
        .onError([](const char *message)
                { std::cout << "声明交换机失败: " << message << std::endl; })
        .onSuccess([]()
                { std::cout << "test-exchange 交换机创建成功" << std::endl; });
    
    // 6.声明队列
    channel.declareQueue("test-queue")
        .onError([](const char *message)
                 { std::cout << "声明队列失败: " << message << std::endl; })
        .onSuccess([]()
                 { std::cout << "test-queue 队列创建成功" << std::endl; });

    // 7.针对交换机和队列进行绑定
    channel.bindQueue("test-exchange", "test-queue", "test-queue-key")
        .onError([](const char *message)
                 { std::cout << "test-exchange - test-queue 绑定失败: " << message << std::endl; })
        .onSuccess([]()
                 { std::cout << "test-exchange - test-queue 绑定成功" << std::endl; });

    // 8.向交换机发布消息
    for (int i = 0; i < 5; ++i)
    {
        std::string msg = "Hello SnowK-" + std::to_string(i);
        if(channel.publish("test-exchange", "test-queue-key", msg) == false)
        {
            std::cout << "publish 失败" << std::endl;
        }
    }

    // 9.启动底层网络通信框架 -> 开启IO
    ev_run(loop, 0);

    return 0;
}