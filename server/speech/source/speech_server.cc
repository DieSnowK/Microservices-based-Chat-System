#include "speech_server.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(registry_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(instance_name, "/speech_service/instance", "Name of the current instance");
DEFINE_string(access_host, "127.0.0.1:10001", "The external access address of the current instance");

DEFINE_int32(listen_port, 10001, "The RPC server listens on the port");
DEFINE_int32(rpc_timeout, -1, "RPC call timeout period");
DEFINE_int32(rpc_threads, 1, "The number of I/O threads of the rpc");

DEFINE_string(app_id, "60694095", "Speech platform app ID");
DEFINE_string(api_key, "PWn6zlsxym8VwpBW8Or4PPGe", "Speech platform API key");
DEFINE_string(secret_key, "Bl0mn74iyAkr3FzCo5TZV7lBq7NYoms9", "Speech platform secret key");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    SnowK::SpeechServerBuilder ssb;
    ssb.Make_Asr_Object(FLAGS_app_id, FLAGS_api_key, FLAGS_secret_key);
    ssb.Make_RPC_Server(FLAGS_listen_port, FLAGS_rpc_timeout, FLAGS_rpc_threads);
    ssb.Make_Reg_Object(FLAGS_registry_host, FLAGS_base_service + FLAGS_instance_name, FLAGS_access_host);

    auto server = ssb.Build();
    server->Start();
    
    return 0;
}