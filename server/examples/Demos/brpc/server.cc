#include <brpc/server.h>
#include <butil/logging.h>
#include "main.pb.h"

// 继承于EchoService, 创建一个子类, 并实现RPC调用的业务功能
class EchoServiceImpl : public SnowK::EchoService
{
public:
    EchoServiceImpl()
    {}

    ~EchoServiceImpl()
    {}

    void Echo(google::protobuf::RpcController *controller,
              const ::SnowK::EchoRequest *request,
              ::SnowK::EchoResponse *response,
              ::google::protobuf::Closure *done)
    {
        brpc::ClosureGuard rpc_guard(done);
        std::cout << "收到消息: " << request->message() << std::endl;

        std::string str = request->message() + "--响应";
        response->set_message(str);

        // done->Run(); // 已经有了ClosureGuard, 则不再需要
    }
};

int main(int argc, char* argv[])
{
    // 0.关闭brpc的默认日志输出
    logging::LoggingSettings settings;
    settings.logging_dest = logging::LoggingDestination::LOG_TO_NONE;
    logging::InitLogging(settings);

    // 1.构造服务器对象
    brpc::Server server;

    // 2.向服务器对象中，新增EchoService服务
    EchoServiceImpl echo_service;
    if (server.AddService(&echo_service, brpc::ServiceOwnership::SERVER_DOESNT_OWN_SERVICE) == -1)
    {
        std::cout << "添加Rpc服务失败!" << std::endl;
        return -1;
    }

    // 3.启动服务器
    brpc::ServerOptions options;
    options.idle_timeout_sec = -1; // 连接空闲超时时间, 超时后连接被关闭
    options.num_threads = 1;       // IO线程数量

    if(server.Start(3366, &options) == -1) // 开始监听 IP:PORT
    {
        std::cout << "启动服务器失败" << std::endl;
        return -1;
    }

    server.RunUntilAskedToQuit(); // 修改等待运行结束

    return 0;
}