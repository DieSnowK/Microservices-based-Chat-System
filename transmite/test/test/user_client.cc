#include <thread>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include "etcd.hpp"
#include "channel.hpp"
#include "utils.hpp"
#include "user.pb.h"
#include "base.pb.h"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(user_service, "/service/user_service", "Name of the current instance");

SnowK::ServiceManager::ptr user_channels;

void Reg_User(const std::string &nickname, const std::string &pwd)
{
    auto channel = user_channels->Choose(FLAGS_user_service); // 获取通信信道

    brpc::Controller cntl;
    SnowK::UserRegisterReq req;
    SnowK::UserRegisterRsp rsp;
    req.set_request_id(SnowK::UUID());
    req.set_nickname(nickname);
    req.set_password(pwd);

    SnowK::UserService_Stub stub(channel.get());
    stub.UserRegister(&cntl, &req, &rsp, nullptr);
}

void Set_User_Avatar(const std::string &uid, const std::string &avatar)
{
    auto channel = user_channels->Choose(FLAGS_user_service);

    brpc::Controller cntl;
    SnowK::SetUserAvatarRsp rsp;
    SnowK::SetUserAvatarReq req;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid);
    req.set_session_id("测试登录会话ID");
    req.set_avatar(avatar);

    SnowK::UserService_Stub stub(channel.get());
    stub.SetUserAvatar(&cntl, &req, &rsp, nullptr);
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    user_channels = std::make_shared<SnowK::ServiceManager>();

    user_channels->Declare(FLAGS_user_service);
    auto put_cb = std::bind(&SnowK::ServiceManager::ServiceOnline, user_channels.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&SnowK::ServiceManager::ServiceOffline, user_channels.get(), std::placeholders::_1, std::placeholders::_2);

    SnowK::Discovery::ptr dclient = std::make_shared<SnowK::Discovery>(FLAGS_etcd_host, FLAGS_base_service, put_cb, del_cb);

    // Reg_User("SnowK", "123456");
    // Reg_User("DieSnowK", "123456");

    // std::string uid1, uid2;
    // std::cout << "输入佩奇用户ID:";
    // std::fflush(stdout);
    // std::cin >> uid1;
    // std::cout << "输入乔治用户ID:";
    // std::fflush(stdout);
    // std::cin >> uid2;

    Set_User_Avatar("731f500868840000", "SnowK Avatar");
    Set_User_Avatar("c4dc68239a9a0001", "DieSnowK Avatar");

    return 0;
}