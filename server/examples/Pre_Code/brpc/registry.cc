#include <gflags/gflags.h>
#include <brpc/server.h>
#include <butil/logging.h>
#include "../etcd/etcd.hpp"
#include "main.pb.h"


DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(instance_name, "/echo/instance", "Name of the current instance");
DEFINE_string(access_host, "127.0.0.1:3366", "The external access address of the current instance");
DEFINE_int32(listen_port, 3366, "The RPC server listens on the port");

class EchoServiceImpl : public SnowK::EchoService
{
public:
    EchoServiceImpl()
    {}

    ~EchoServiceImpl()
    {}

    void Echo(google::protobuf::RpcController *controller,
              const ::SnowK::EchoRequest *request,
              ::SnowK::EchoResponse *response,
              ::google::protobuf::Closure *done)
    {
        brpc::ClosureGuard rpc_guard(done);
        std::cout << "Message received: " << request->message() << std::endl;

        std::string str = request->message() + "--Response";
        response->set_message(str);

        // done->Run();
    }
};

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    logging::LoggingSettings settings;
    settings.logging_dest = logging::LoggingDestination::LOG_TO_NONE;
    logging::InitLogging(settings);

    brpc::Server server;

    // Add the EchoService service to the server object
    EchoServiceImpl echo_service;
    if(server.AddService(&echo_service, brpc::ServiceOwnership::SERVER_DOESNT_OWN_SERVICE) == -1)
    {
        LOG_ERROR("Failed to add RPC service");
        return -1;
    }

    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;
    options.num_threads = 1;

    if(server.Start(FLAGS_listen_port, &options) == -1)
    {
        LOG_ERROR("Failed to start the server");
        return -1;
    }

    // Register the service with the etcd server
    SnowK::Registry::ptr rClient = std::make_shared<SnowK::Registry>(FLAGS_etcd_host);

    // At the time of registration: /service/echo/instance
    // At the time of discovery: /service/echo
    rClient->Registry_Service(FLAGS_base_service + FLAGS_instance_name, FLAGS_access_host);
    LOG_DEBUG("Service name-host: {}-{}", FLAGS_base_service + FLAGS_instance_name, FLAGS_access_host);

    server.RunUntilAskedToQuit();

    return 0;
}