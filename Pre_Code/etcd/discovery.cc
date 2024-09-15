#include <gflags/gflags.h>
#include "etcd.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");

void Online(const std::string &service_name, const std::string &service_host)
{
    LOG_DEBUG("On-line service: {}-{}", service_name, service_host);
}

void Offline(const std::string &service_name, const std::string &service_host)
{
    LOG_DEBUG("Off-line service: {}-{}", service_name, service_host);
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    SnowK::Discovery::ptr dClient = std::make_shared<SnowK::Discovery>(FLAGS_etcd_host, FLAGS_base_service, Online, Offline);

    std::this_thread::sleep_for(std::chrono::seconds(300));

    return 0;
}