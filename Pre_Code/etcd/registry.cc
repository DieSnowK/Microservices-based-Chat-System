#include <gflags/gflags.h>
#include "etcd.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(instance_name, "/user/instance", "Name of the current instance");
DEFINE_string(access_host, "127.0.0.1:3366", "The external access address of the current instance");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    SnowK::Registry::ptr rClient = std::make_shared<SnowK::Registry>(FLAGS_etcd_host);
    rClient->Registry_Service(FLAGS_base_service + FLAGS_instance_name, FLAGS_access_host);
    LOG_DEBUG("Service name: {}", FLAGS_base_service + FLAGS_instance_name);

    std::this_thread::sleep_for(std::chrono::seconds(300));
    
    return 0;
}