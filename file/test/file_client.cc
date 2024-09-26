#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <thread>
#include "etcd.hpp"
#include "channel.hpp"
#include "logger.hpp"
#include "file.pb.h"
#include "base.pb.h"
#include "utils.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(registry_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(file_service, "/service/file_service", "Name of the current instance");

SnowK::ServiceChannel::ChannelPtr channel;
std::string single_file_id;

TEST(PUT_TEST, SINGLE_FILE)
{
    std::string body;
    ASSERT_TRUE(SnowK::ReadFile("./Makefile", body));

    SnowK::FileService_Stub stub(channel.get());

    SnowK::PutSingleFileReq req;
    req.set_request_id("1111");
    req.mutable_file_data()->set_file_name("Makefile");
    req.mutable_file_data()->set_file_size(body.size());
    req.mutable_file_data()->set_file_content(body);

    brpc::Controller *cntl = new brpc::Controller();
    SnowK::PutSingleFileRsp *rsp = new SnowK::PutSingleFileRsp();
    stub.PutSingleFile(cntl, &req, rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());

    ASSERT_TRUE(rsp->success());
    ASSERT_EQ(rsp->file_info().file_size(), body.size());
    ASSERT_EQ(rsp->file_info().file_name(), "Makefile");
    single_file_id = rsp->file_info().file_id();
    LOG_DEBUG("File ID: {}", rsp->file_info().file_id());
}

TEST(get_test, single_file)
{
    SnowK::FileService_Stub stub(channel.get());
    SnowK::GetSingleFileReq req;
    SnowK::GetSingleFileRsp *rsp;
    req.set_request_id("2222");
    req.set_file_id(single_file_id);

    brpc::Controller *cntl = new brpc::Controller();
    rsp = new SnowK::GetSingleFileRsp();
    stub.GetSingleFile(cntl, &req, rsp, nullptr);

    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp->success());

    ASSERT_EQ(single_file_id, rsp->file_data().file_id());
    SnowK::WriteFile("make_file_download", rsp->file_data().file_content());
}

std::vector<std::string> multi_file_id;

TEST(put_test, multi_file)
{
    std::string body1;
    ASSERT_TRUE(SnowK::ReadFile("./base.pb.h", body1));
    std::string body2;
    ASSERT_TRUE(SnowK::ReadFile("./file.pb.h", body2));

    SnowK::FileService_Stub stub(channel.get());

    SnowK::PutMultiFileReq req;
    req.set_request_id("3333");

    auto file_data = req.add_file_data();
    file_data->set_file_name("base.pb.h");
    file_data->set_file_size(body1.size());
    file_data->set_file_content(body1);

    file_data = req.add_file_data();
    file_data->set_file_name("file.pb.h");
    file_data->set_file_size(body2.size());
    file_data->set_file_content(body2);

    brpc::Controller *cntl = new brpc::Controller();
    SnowK::PutMultiFileRsp *rsp = new SnowK::PutMultiFileRsp();
    stub.PutMultiFile(cntl, &req, rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp->success());

    for (int i = 0; i < rsp->file_info_size(); i++)
    {
        multi_file_id.push_back(rsp->file_info(i).file_id());
        LOG_DEBUG("File ID: {}", multi_file_id[i]);
    }
}

TEST(get_test, multi_file)
{
    SnowK::FileService_Stub stub(channel.get());
    SnowK::GetMultiFileReq req;
    SnowK::GetMultiFileRsp *rsp;
    req.set_request_id("4444");
    req.add_file_id_list(multi_file_id[0]);
    req.add_file_id_list(multi_file_id[1]);

    brpc::Controller *cntl = new brpc::Controller();
    rsp = new SnowK::GetMultiFileRsp();
    stub.GetMultiFile(cntl, &req, rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp->success());

    ASSERT_TRUE(rsp->file_data().find(multi_file_id[0]) != rsp->file_data().end());
    ASSERT_TRUE(rsp->file_data().find(multi_file_id[1]) != rsp->file_data().end());
    
    auto map = rsp->file_data();
    auto file_data1 = map[multi_file_id[0]];
    SnowK::WriteFile("base_download_file1", file_data1.file_content());
    auto file_data2 = map[multi_file_id[1]];
    SnowK::WriteFile("file_download_file2", file_data2.file_content());
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    auto sm = std::make_shared<SnowK::ServiceManager>();
    sm->Declare(FLAGS_file_service);
    auto put_cb = std::bind(&SnowK::ServiceManager::ServiceOnline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&SnowK::ServiceManager::ServiceOffline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    SnowK::Discovery::ptr dclient = std::make_shared<SnowK::Discovery>(FLAGS_registry_host, FLAGS_base_service, put_cb, del_cb);

    channel = sm->Choose(FLAGS_file_service);

    return RUN_ALL_TESTS();
}