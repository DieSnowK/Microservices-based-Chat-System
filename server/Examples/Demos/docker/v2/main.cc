#include <iostream>
#include <gflags/gflags.h>

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    std::cout << "Hello SnowK-Docker" << std::endl;
    return 0;
}