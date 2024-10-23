#include "gateway_server.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_int32(http_listen_port, 9000, "HTTP server listens on the port");
DEFINE_int32(websocket_listen_port, 9001, "Websocket server listens on the port");

DEFINE_string(registry_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(file_service, "/service/file_service", "The name of the file management subservice");
DEFINE_string(user_service, "/service/user_service", "The name of the user management subservice");
DEFINE_string(friend_service, "/service/friend_service", "The name of the friend management subservice");
DEFINE_string(message_service, "/service/message_service", "The name of the message management subservice");
DEFINE_string(speech_service, "/service/speech_service", "The name of the speech management subservice");
DEFINE_string(transmite_service, "/service/transmite_service", "The name of the transmite management subservice");

DEFINE_string(redis_host, "127.0.0.1", "Redis access address");
DEFINE_int32(redis_port, 6379, "Access port of the Redis server");
DEFINE_int32(redis_db, 0, "The default name of the Redis database");
DEFINE_bool(redis_keep_alive, true, "Redis persistent keep-alive option");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    SnowK::GatewayServerBuilder gsb;
    gsb.Make_Redis_Object(FLAGS_redis_host, FLAGS_redis_port, FLAGS_redis_db, FLAGS_redis_keep_alive);
    gsb.Make_Discovery_Object(FLAGS_registry_host, FLAGS_base_service, FLAGS_file_service,
                              FLAGS_speech_service, FLAGS_message_service, FLAGS_friend_service,
                              FLAGS_user_service, FLAGS_transmite_service);
    gsb.Make_Server_Object(FLAGS_websocket_listen_port, FLAGS_http_listen_port);

    auto server = gsb.Build();
    server->Start();

    return 0;
}