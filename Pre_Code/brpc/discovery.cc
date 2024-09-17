#include <gflags/gflags.h>
#include "../etcd/etcd.hpp"
#include "main.pb.h"
#include "channel.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(call_service, "/service/echo", "The root directory of service monitoring");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    // 1.Construct an RPC channel management object
    auto svr_mgr = std::make_shared<SnowK::ServiceManager>();
    svr_mgr->Declare(FLAGS_call_service);

    auto put_cb = std::bind(&SnowK::ServiceManager::ServiceOnline, svr_mgr.get(), 
                            std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&SnowK::ServiceManager::ServiceOffline, svr_mgr.get(),
                            std::placeholders::_1, std::placeholders::_2);

    // 2.Construct a service discovery object
    auto dClient = std::make_shared<SnowK::Discovery>(FLAGS_etcd_host, FLAGS_base_service, put_cb, del_cb);

    while(1)
    {
        // 3.Obtain the channel that provides the Echo service through the RPC channel management object
        auto channel = svr_mgr->Choose(FLAGS_call_service);
        if(!channel)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // return -1;
            continue;
        }

        // 4.Initiate an Echo Rpc call
        SnowK::EchoService_Stub stub(channel.get());
        SnowK::EchoRequest req;
        req.set_message("Hello SnowK");

        brpc::Controller *cntl = new brpc::Controller();
        SnowK::EchoResponse *resp = new SnowK::EchoResponse();

        stub.Echo(cntl, &req, resp, nullptr);
        if(cntl->Failed())
        {
            LOG_ERROR("The RPC call failed: {}", cntl->ErrorText());
            delete cntl;
            delete resp;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        std::cout << "Response: " << resp->message() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}