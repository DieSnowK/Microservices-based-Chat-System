#include <thread>
#include <gflags/gflags.h>
#include "etcd.hpp"
#include "channel.hpp"
#include "utils.hpp"
#include "transmite.pb.h"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(transmite_service, "/service/transmite_service", "Name of the current instance");

SnowK::ServiceManager::ptr sm;

void String_Message(const std::string &uid, const std::string &sid, const std::string &msg)
{
    auto channel = sm->Choose(FLAGS_transmite_service);

    brpc::Controller cntl;
    SnowK::NewMessageReq req;
    SnowK::GetTransmitTargetRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_message()->set_message_type(SnowK::MessageType::STRING);
    req.mutable_message()->mutable_string_message()->set_content(msg);

    SnowK::MsgTransmitService_Stub stub(channel.get());
    stub.GetTransmitTarget(&cntl, &req, &rsp, nullptr);
}

void Image_Message(const std::string &uid, const std::string &sid, const std::string &msg)
{
    auto channel = sm->Choose(FLAGS_transmite_service);

    brpc::Controller cntl;
    SnowK::NewMessageReq req;
    SnowK::GetTransmitTargetRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_message()->set_message_type(SnowK::MessageType::IMAGE);
    req.mutable_message()->mutable_image_message()->set_image_content(msg);

    SnowK::MsgTransmitService_Stub stub(channel.get());
    stub.GetTransmitTarget(&cntl, &req, &rsp, nullptr);
}

void Speech_Message(const std::string &uid, const std::string &sid, const std::string &msg)
{
    auto channel = sm->Choose(FLAGS_transmite_service);

    brpc::Controller cntl;
    SnowK::NewMessageReq req;
    SnowK::GetTransmitTargetRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_message()->set_message_type(SnowK::MessageType::SPEECH);
    req.mutable_message()->mutable_speech_message()->set_file_contents(msg);

    SnowK::MsgTransmitService_Stub stub(channel.get());
    stub.GetTransmitTarget(&cntl, &req, &rsp, nullptr);
}

void File_Message(const std::string &uid, const std::string &sid,
                  const std::string &filename, const std::string &content)
{
    auto channel = sm->Choose(FLAGS_transmite_service);

    brpc::Controller cntl;
    SnowK::NewMessageReq req;
    SnowK::GetTransmitTargetRsp rsp;

    req.set_request_id(SnowK::UUID());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_message()->set_message_type(SnowK::MessageType::FILE);
    req.mutable_message()->mutable_file_message()->set_file_contents(content);
    req.mutable_message()->mutable_file_message()->set_file_name(filename);
    req.mutable_message()->mutable_file_message()->set_file_size(content.size());

    SnowK::MsgTransmitService_Stub stub(channel.get());
    stub.GetTransmitTarget(&cntl, &req, &rsp, nullptr);
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    sm = std::make_shared<SnowK::ServiceManager>();
    sm->Declare(FLAGS_transmite_service);
    auto put_cb = std::bind(&SnowK::ServiceManager::ServiceOnline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&SnowK::ServiceManager::ServiceOffline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    SnowK::Discovery::ptr dclient = std::make_shared<SnowK::Discovery>(FLAGS_etcd_host, FLAGS_base_service, put_cb, del_cb);

    String_Message("c4dc68239a9a0001", "会话ID1", "I'm fine, thank you");
    Image_Message("731f500868840000", "会话ID1", "Slim Icon");
    Speech_Message("731f500868840000", "会话ID1", "hai hai hai");
    File_Message("731f500868840000", "0d90-755571d8-0003", "GTA5", "Start~");

    return 0;
}