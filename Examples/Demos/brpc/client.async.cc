#include <thread>
#include <brpc/channel.h>
#include "main.pb.h"

void Callback(brpc::Controller* cntl, SnowK::EchoResponse* resp)
{
    std::shared_ptr<brpc::Controller> cntl_guard(cntl);
    std::shared_ptr<SnowK::EchoResponse> resp_guard(resp);

    if (cntl->Failed())
    {
        std::cout << "RPC调用失败" << cntl->ErrorText() << std::endl;
        return;
    }

    std::cout << "收到响应: " << resp->message() << std::endl;
}

int main(int argc, char *argv[])
{
    // 1.构造Channel信道, 连接服务器
    brpc::ChannelOptions options;
    options.connect_timeout_ms = -1; // 连接等待超时时间, -1表示一直等待
    options.timeout_ms = -1;         // RPC请求等待超时时间, -1表示一直等待
    options.max_retry = 3;           // 请求重试次数
    options.protocol = "baidu_std";  // 序列化协议, 默认使用baidu_std

    brpc::Channel channel;
    if (channel.Init("127.0.0.1:3366", &options) == -1)
    {
        std::cout << "初始化信道失败" << std::endl;
        return -1;
    }

    // 2.构建EchoService_Stub对象, 用于进行RPC调用
    SnowK::EchoService_Stub stub(&channel);

    // 3.进行RPC调用
    SnowK::EchoRequest req;
    req.set_message("Hello SnowK");

    brpc::Controller *cntl = new brpc::Controller();
    SnowK::EchoResponse *resp = new SnowK::EchoResponse();
    auto closure = google::protobuf::NewCallback(Callback, cntl, resp);
    
    stub.Echo(cntl, &req, resp, closure); // 异步调用

    std::cout << "异步调用结束" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}