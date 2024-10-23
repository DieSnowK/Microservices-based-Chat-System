#include <iostream>
#include <gflags/gflags.h>

DEFINE_bool(debug, true, "是否开启DEBUG模式, 格式:true/false");
DEFINE_string(ip, "127.0.0.1", "服务器监听IP, 格式: 127.0.0.1");
DEFINE_int32(port, 3366, "服务器监听端口, 格式: 3366");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    std::cout << FLAGS_debug << std::endl;
    std::cout << FLAGS_ip << std::endl;
    std::cout << FLAGS_port << std::endl;

    return 0;
}
