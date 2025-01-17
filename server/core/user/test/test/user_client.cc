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

SnowK::ServiceChannel::ChannelPtr channel;

SnowK::UserInfo user_info;
std::string login_ssid;
std::string new_nickname = "Ugly";

// TEST(User_subservice_test, UserRegister)
// {
//     user_info.set_nickname("Fashion");

//     SnowK::UserRegisterReq req;
//     req.set_request_id(SnowK::UUID());
//     req.set_nickname(user_info.nickname());
//     req.set_password("123456");

//     SnowK::UserRegisterRsp rsp;
//     brpc::Controller cntl;
//     SnowK::UserService_Stub stub(channel.get());
//     stub.UserRegister(&cntl, &req, &rsp, nullptr);

//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }

// TEST(User_subservice_test, UserLogin)
// {
//     SnowK::UserLoginReq req;
//     req.set_request_id(SnowK::UUID());
//     req.set_nickname("Ugly");
//     req.set_password("123456");

//     SnowK::UserLoginRsp rsp;
//     brpc::Controller cntl;
//     SnowK::UserService_Stub stub(channel.get());
//     stub.UserLogin(&cntl, &req, &rsp, nullptr);

//     // std::cout << rsp.success() << " " << rsp.errmsg() << std::endl;

//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());

//     login_ssid = rsp.login_session_id();
// }

// TEST(User_subservice_test, SetUserAvatar)
// {
//     SnowK::SetUserAvatarReq req;
//     req.set_request_id(SnowK::UUID());
//     req.set_user_id(user_info.user_id());
//     req.set_session_id(login_ssid);
//     req.set_avatar(user_info.avatar());

//     SnowK::SetUserAvatarRsp rsp;
//     brpc::Controller cntl;
//     SnowK::UserService_Stub stub(channel.get());
//     stub.SetUserAvatar(&cntl, &req, &rsp, nullptr);

//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }

// TEST(User_subservice_test, SetUserDescription)
// {
//     SnowK::SetUserDescriptionReq req;
//     req.set_request_id(SnowK::UUID());
//     req.set_user_id(user_info.user_id());
//     req.set_session_id(login_ssid);
//     req.set_description(user_info.description());

//     SnowK::SetUserDescriptionRsp rsp;
//     brpc::Controller cntl;
//     SnowK::UserService_Stub stub(channel.get());
//     stub.SetUserDescription(&cntl, &req, &rsp, nullptr);

//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }

// TEST(User_subservice_test, SetUserNickname)
// {
//     SnowK::SetUserNicknameReq req;
//     req.set_request_id(SnowK::UUID());
//     req.set_user_id(user_info.user_id());
//     req.set_session_id(login_ssid);
//     req.set_nickname(new_nickname);

//     SnowK::SetUserNicknameRsp rsp;
//     brpc::Controller cntl;
//     SnowK::UserService_Stub stub(channel.get());
//     stub.SetUserNickname(&cntl, &req, &rsp, nullptr);

//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());

//     user_info.set_nickname(new_nickname);
// }

// TEST(User_subservice_test, GetUserInfo)
// {
//     SnowK::GetUserInfoReq req;
//     req.set_request_id(SnowK::UUID());
//     req.set_user_id(user_info.user_id());
//     req.set_session_id(login_ssid);

//     SnowK::GetUserInfoRsp rsp;
//     brpc::Controller cntl;
//     SnowK::UserService_Stub stub(channel.get());
//     stub.GetUserInfo(&cntl, &req, &rsp, nullptr);

//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
//     ASSERT_EQ(user_info.user_id(), rsp.user_info().user_id());
//     ASSERT_EQ(new_nickname, rsp.user_info().nickname());
//     ASSERT_EQ(user_info.description(), rsp.user_info().description());
//     ASSERT_EQ("", rsp.user_info().phone());
//     ASSERT_EQ(user_info.avatar(), rsp.user_info().avatar());
// }

void set_user_avatar(const std::string &uid, const std::string &avatar)
{
    SnowK::SetUserAvatarReq req;
    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid);
    req.set_session_id(login_ssid);
    req.set_avatar(avatar);

    SnowK::SetUserAvatarRsp rsp;
    brpc::Controller cntl;
    SnowK::UserService_Stub stub(channel.get());
    stub.SetUserAvatar(&cntl, &req, &rsp, nullptr);

    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());
}

// TEST(User_subservice_test, GetMultiUserInfo)
// {
//     set_user_avatar("uid1", "user1 avatar");
//     set_user_avatar("uid2", "DSK avatar");

//     SnowK::GetMultiUserInfoReq req;
//     req.set_request_id(SnowK::UUID());
//     req.add_users_id("uid1");
//     req.add_users_id("uid2");
//     req.add_users_id("d2a8f4bd3e410000");

//     SnowK::GetMultiUserInfoRsp rsp;
//     brpc::Controller cntl;
//     SnowK::UserService_Stub stub(channel.get());
//     stub.GetMultiUserInfo(&cntl, &req, &rsp, nullptr);

//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());

//     auto users_map = rsp.mutable_users_info();
//     SnowK::UserInfo fuser = (*users_map)["d2a8f4bd3e410000"];
//     ASSERT_EQ(fuser.user_id(), "d2a8f4bd3e410000");
//     ASSERT_EQ(fuser.nickname(), "Ugly");
//     ASSERT_EQ(fuser.description(), "Cool Boy");
//     ASSERT_EQ(fuser.phone(), "");
//     ASSERT_EQ(fuser.avatar(), "Handsome Icon");

//     SnowK::UserInfo puser = (*users_map)["uid1"];
//     ASSERT_EQ(puser.user_id(), "uid1");
//     ASSERT_EQ(puser.nickname(), "昵称1");
//     ASSERT_EQ(puser.description(), "SnowK~");
//     ASSERT_EQ(puser.phone(), "");
//     ASSERT_EQ(puser.avatar(), "user1 avatar");

//     SnowK::UserInfo quser = (*users_map)["uid2"];
//     ASSERT_EQ(quser.user_id(), "uid2");
//     ASSERT_EQ(quser.nickname(), "DSK");
//     ASSERT_EQ(quser.description(), "");
//     ASSERT_EQ(quser.phone(), "15566667777");
//     ASSERT_EQ(quser.avatar(), "DSK avatar");
// }

std::string code_id;
void Get_Code()
{
    SnowK::PhoneVerifyCodeReq req;
    req.set_request_id(SnowK::UUID());
    req.set_phone_number(user_info.phone());

    SnowK::PhoneVerifyCodeRsp rsp;
    brpc::Controller cntl;
    SnowK::UserService_Stub stub(channel.get());
    stub.GetPhoneVerifyCode(&cntl, &req, &rsp, nullptr);

    ASSERT_FALSE(cntl.Failed());
    ASSERT_TRUE(rsp.success());

    code_id = rsp.verify_code_id();
}

// TEST(User_subservice_test, PhoneRegister)
// {
//     Get_Code();

//     SnowK::PhoneRegisterReq req;
//     req.set_request_id(SnowK::UUID());
//     req.set_phone_number(user_info.phone());
//     req.set_verify_code_id(code_id);

//     std::cout << "PhoneRegister, input verify code:> " << std::endl;
//     std::string code;
//     std::cin >> code;
//     req.set_verify_code(code);

//     SnowK::PhoneRegisterRsp rsp;
//     brpc::Controller cntl;
//     SnowK::UserService_Stub stub(channel.get());
//     stub.PhoneRegister(&cntl, &req, &rsp, nullptr);

//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }

// TEST(User_subservice_test, PhoneLogin)
// {
//     std::this_thread::sleep_for(std::chrono::seconds(3));
//     Get_Code();

//     SnowK::PhoneLoginReq req;
//     req.set_request_id(SnowK::UUID());
//     req.set_phone_number(user_info.phone());
//     req.set_verify_code_id(code_id);

//     std::cout << "PhoneLogin, input verify code:> " << std::endl;
//     std::string code;
//     std::cin >> code;
//     req.set_verify_code(code);

//     SnowK::PhoneLoginRsp rsp;
//     brpc::Controller cntl;
//     SnowK::UserService_Stub stub(channel.get());
//     stub.PhoneLogin(&cntl, &req, &rsp, nullptr);

//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());

//     std::cout << "PhoneLogin session id: " << rsp.login_session_id() << std::endl;
// }

// TEST(User_subservice_test, SetUserPhoneNumber)
// {
//     // std::this_thread::sleep_for(std::chrono::seconds(10));
//     Get_Code();

//     SnowK::SetUserPhoneNumberReq req;
//     req.set_request_id(SnowK::UUID());

//     std::cout << "SetUserPhoneNumber, inout user_id:> " << std::endl;
//     std::string user_id;
//     std::cin >> user_id;
//     req.set_user_id(user_id);

//     req.set_phone_number("18888888888");
//     req.set_phone_verify_code_id(code_id);
//     std::cout << "SetUserPhoneNumber, input verify code:> " << std::endl;
//     std::string code;
//     std::cin >> code;
//     req.set_phone_verify_code(code);

//     SnowK::SetUserPhoneNumberRsp rsp;
//     brpc::Controller cntl;
//     SnowK::UserService_Stub stub(channel.get());
//     stub.SetUserPhoneNumber(&cntl, &req, &rsp, nullptr);

//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    testing::InitGoogleTest(&argc, argv);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    auto user_channels = std::make_shared<SnowK::ServiceManager>();
    user_channels->Declare(FLAGS_user_service);
    auto put_cb = std::bind(&SnowK::ServiceManager::ServiceOnline, user_channels.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&SnowK::ServiceManager::ServiceOffline, user_channels.get(), std::placeholders::_1, std::placeholders::_2);
    SnowK::Discovery::ptr dclient = std::make_shared<SnowK::Discovery>(FLAGS_etcd_host, FLAGS_base_service, put_cb, del_cb);
    channel = user_channels->Choose(FLAGS_user_service);

    user_info.set_user_id("d2a8f4bd3e410000");
    user_info.set_nickname("Fashion");
    user_info.set_description("Cool Boy");
    user_info.set_phone("");
    user_info.set_avatar("Handsome Icon");
    
    return RUN_ALL_TESTS();
}