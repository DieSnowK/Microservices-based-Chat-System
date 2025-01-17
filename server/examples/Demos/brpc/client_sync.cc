#include <brpc/channel.h>
#include "main.pb.h"

int main(int argc, char* argv[])
{
    // 1.构造Channel信道, 连接服务器
    brpc::ChannelOptions options;
    options.connect_timeout_ms = -1; // 连接等待超时时间, -1表示一直等待
    options.timeout_ms = -1;         // RPC请求等待超时时间, -1表示一直等待
    options.max_retry = 3;           // 请求重试次数
    options.protocol = "baidu_std";  // 序列化协议, 默认使用baidu_std

    brpc::Channel channel;
    if(channel.Init("127.0.0.1:3366", &options) == -1)
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
    stub.Echo(cntl, &req, resp, nullptr); // nullptr为同步调用
    if(cntl->Failed())
    {
        std::cout << "RPC调用失败" << cntl->ErrorText() << std::endl;
        return -1;
    }

    std::cout << "收到响应: " << resp->message() << std::endl;

    delete cntl;
    delete resp;

    return 0;
}