#include <thread>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "etcd.hpp"
#include "channel.hpp"
#include "utils.hpp"
#include "message.pb.h"
#include "base.pb.h"
#include "user.pb.h"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(message_service, "/service/message_service", "The root directory of service monitoring");

SnowK::ServiceManager::ptr sm;

void Range_Test(const std::string &ssid,
                const boost::posix_time::ptime &stime,
                const boost::posix_time::ptime &etime)
{
    auto channel = sm->Choose(FLAGS_message_service);
    SnowK::MsgStorageService_Stub stub(channel.get());

    brpc::Controller cntl;
    SnowK::GetHistoryMsgReq req;
    SnowK::GetHistoryMsgRsp rsp;
    req.set_request_id(SnowK::UUID());
    req.set_chat_session_id(ssid);
    req.set_start_time(boost::posix_time::to_time_t(stime));
    req.set_over_time(boost::posix_time::to_time_t(etime));

    stub.GetHistoryMsg(&cntl, &req, &rsp, nullptr);
    for (int i = 0; i < rsp.msg_list_size(); i++)
    {
        std::cout << "-----------------------获取区间消息--------------------------\n";
        auto msg = rsp.msg_list(i);
        std::cout << msg.message_id() << std::endl;
        std::cout << msg.chat_session_id() << std::endl;
        std::cout << boost::posix_time::to_simple_string(boost::posix_time::from_time_t(msg.timestamp())) << std::endl;
        std::cout << msg.sender().user_id() << std::endl;
        std::cout << msg.sender().nickname() << std::endl;
        std::cout << msg.sender().avatar() << std::endl;
        if (msg.message().message_type() == SnowK::MessageType::STRING)
        {
            std::cout << "文本消息：" << msg.message().string_message().content() << std::endl;
        }
        else if (msg.message().message_type() == SnowK::MessageType::IMAGE)
        {
            std::cout << "图片消息：" << msg.message().image_message().image_content() << std::endl;
        }
        else if (msg.message().message_type() == SnowK::MessageType::FILE)
        {
            std::cout << "文件消息：" << msg.message().file_message().file_contents() << std::endl;
            std::cout << "文件名称：" << msg.message().file_message().file_name() << std::endl;
        }
        else if (msg.message().message_type() == SnowK::MessageType::SPEECH)
        {
            std::cout << "语音消息：" << msg.message().speech_message().file_contents() << std::endl;
        }
        else
        {
            std::cout << "类型错误！！\n";
        }
    }
}

void Recent_Test(const std::string &ssid, int count)
{
    auto channel = sm->Choose(FLAGS_message_service);
    SnowK::MsgStorageService_Stub stub(channel.get());

    brpc::Controller cntl;
    SnowK::GetRecentMsgReq req;
    SnowK::GetRecentMsgRsp rsp;
    req.set_request_id(SnowK::UUID());
    req.set_chat_session_id(ssid);
    req.set_msg_count(count);

    stub.GetRecentMsg(&cntl, &req, &rsp, nullptr);
    for (int i = 0; i < rsp.msg_list_size(); i++)
    {
        std::cout << "----------------------获取最近消息---------------------------\n";
        auto msg = rsp.msg_list(i);
        std::cout << msg.message_id() << std::endl;
        std::cout << msg.chat_session_id() << std::endl;
        std::cout << boost::posix_time::to_simple_string(boost::posix_time::from_time_t(msg.timestamp())) << std::endl;
        std::cout << msg.sender().user_id() << std::endl;
        std::cout << msg.sender().nickname() << std::endl;
        std::cout << msg.sender().avatar() << std::endl;
        if (msg.message().message_type() == SnowK::MessageType::STRING)
        {
            std::cout << "文本消息：" << msg.message().string_message().content() << std::endl;
        }
        else if (msg.message().message_type() == SnowK::MessageType::IMAGE)
        {
            std::cout << "图片消息：" << msg.message().image_message().image_content() << std::endl;
        }
        else if (msg.message().message_type() == SnowK::MessageType::FILE)
        {
            std::cout << "文件消息：" << msg.message().file_message().file_contents() << std::endl;
            std::cout << "文件名称：" << msg.message().file_message().file_name() << std::endl;
        }
        else if (msg.message().message_type() == SnowK::MessageType::SPEECH)
        {
            std::cout << "语音消息：" << msg.message().speech_message().file_contents() << std::endl;
        }
        else
        {
            std::cout << "类型错误！！\n";
        }
    }
}

void Search_Test(const std::string &ssid, const std::string &key)
{
    auto channel = sm->Choose(FLAGS_message_service);
    SnowK::MsgStorageService_Stub stub(channel.get());

    brpc::Controller cntl;
    SnowK::MsgSearchReq req;
    SnowK::MsgSearchRsp rsp;
    req.set_request_id(SnowK::UUID());
    req.set_chat_session_id(ssid);
    req.set_search_key(key);

    stub.MsgSearch(&cntl, &req, &rsp, nullptr);
    for (int i = 0; i < rsp.msg_list_size(); i++)
    {
        std::cout << "----------------------关键字搜索消息---------------------------\n";
        auto msg = rsp.msg_list(i);
        std::cout << msg.message_id() << std::endl;
        std::cout << msg.chat_session_id() << std::endl;
        std::cout << boost::posix_time::to_simple_string(boost::posix_time::from_time_t(msg.timestamp())) << std::endl;
        std::cout << msg.sender().user_id() << std::endl;
        std::cout << msg.sender().nickname() << std::endl;
        std::cout << msg.sender().avatar() << std::endl;
        if (msg.message().message_type() == SnowK::MessageType::STRING)
        {
            std::cout << "文本消息：" << msg.message().string_message().content() << std::endl;
        }
        else if (msg.message().message_type() == SnowK::MessageType::IMAGE)
        {
            std::cout << "图片消息：" << msg.message().image_message().image_content() << std::endl;
        }
        else if (msg.message().message_type() == SnowK::MessageType::FILE)
        {
            std::cout << "文件消息：" << msg.message().file_message().file_contents() << std::endl;
            std::cout << "文件名称：" << msg.message().file_message().file_name() << std::endl;
        }
        else if (msg.message().message_type() == SnowK::MessageType::SPEECH)
        {
            std::cout << "语音消息：" << msg.message().speech_message().file_contents() << std::endl;
        }
        else
        {
            std::cout << "类型错误！！\n";
        }
    }
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    sm = std::make_shared<SnowK::ServiceManager>();
    sm->Declare(FLAGS_message_service);

    auto put_cb = std::bind(&SnowK::ServiceManager::ServiceOnline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&SnowK::ServiceManager::ServiceOffline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    SnowK::Discovery::ptr dclient = std::make_shared<SnowK::Discovery>(FLAGS_etcd_host, FLAGS_base_service, put_cb, del_cb);

    boost::posix_time::ptime stime(boost::posix_time::time_from_string("2024-10-13 05:44:23"));
    boost::posix_time::ptime etime(boost::posix_time::time_from_string("2024-10-13 05:44:29"));
    Range_Test("会话ID1", stime, etime);
    Recent_Test("会话ID1", 2);
    Search_Test("会话ID1", "thank");
    
    return 0;
}