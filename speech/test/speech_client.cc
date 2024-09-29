#include <gflags/gflags.h>
#include <thread>
#include "etcd.hpp"
#include "channel.hpp"
#include "aip-cpp-sdk/speech.h"
#include "speech.pb.h"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(speech_service, "/service/speech_service", "Name of the current instance");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    auto sm = std::make_shared<SnowK::ServiceManager>();
    sm->Declare(FLAGS_speech_service);

    auto put_cb = std::bind(&SnowK::ServiceManager::ServiceOnline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&SnowK::ServiceManager::ServiceOffline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    SnowK::Discovery::ptr dclient = std::make_shared<SnowK::Discovery>(FLAGS_etcd_host, FLAGS_base_service, put_cb, del_cb);

    auto channel = sm->Choose(FLAGS_speech_service);
    SnowK::SpeechService_Stub stub(channel.get());

    std::string file_content;
    aip::get_file_content("../test/16k.pcm", &file_content);

    SnowK::SpeechRecognitionReq req;
    req.set_speech_content(file_content);
    req.set_request_id("3366");

    brpc::Controller *cntl = new brpc::Controller();
    SnowK::SpeechRecognitionRsp *rsp = new SnowK::SpeechRecognitionRsp();
    stub.SpeechRecognition(cntl, &req, rsp, nullptr);
    if (cntl->Failed() == true)
    {
        std::cout << "Rpc调用失败: " << cntl->ErrorText() << std::endl;
        delete cntl;
        delete rsp;

        return -1;
    }

    if (rsp->success() == false)
    {
        std::cout << rsp->errmsg() << std::endl;
        return -1;
    }

    std::cout << "收到响应: " << rsp->request_id() << std::endl;
    std::cout << "收到响应: " << rsp->recognition_result() << std::endl;

    return 0;
}