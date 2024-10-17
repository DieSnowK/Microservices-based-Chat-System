#include "friend_server.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(registry_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(access_host, "127.0.0.1:10006", "The external access address of the current instance");

DEFINE_int32(listen_port, 10006, "The RPC server listens on the port");
DEFINE_int32(rpc_timeout, -1, "RPC call timeout period");
DEFINE_int32(rpc_threads, 1, "The number of I/O threads of the rpc");

DEFINE_string(instance_name, "/friend_service/instance", "当前实例名称");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(user_service, "/service/user_service", "The name of the file management subservice");
DEFINE_string(message_service, "/service/message_service", "The name of the message management subservice");

DEFINE_string(es_host, "http://127.0.0.1:9200/", "ES search engine server URL");

DEFINE_string(mysql_host, "127.0.0.1", "MySQL access address");
DEFINE_int32(mysql_port, 0, "Access port of the MySQL server");
DEFINE_string(mysql_user, "root", "MySQL server access username");
DEFINE_string(mysql_pwd, "SnowK8989", "MySQL server access password");
DEFINE_string(mysql_db, "SnowK_Test", "The default name of the MySQL database");
DEFINE_string(mysql_cset, "utf8", "MySQL client character set");
DEFINE_int32(mysql_pool_count, 4, "The maximum number of connections in a MySQL connection pool");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    SnowK::FriendServerBuilder fsb;
    fsb.Make_Es_Object({FLAGS_es_host});
    fsb.Make_MySQL_Object(FLAGS_mysql_user, FLAGS_mysql_pwd, FLAGS_mysql_host,
                          FLAGS_mysql_db, FLAGS_mysql_cset, FLAGS_mysql_port, FLAGS_mysql_pool_count);
    fsb.Make_Discovery_Object(FLAGS_registry_host, FLAGS_base_service, FLAGS_user_service, FLAGS_message_service);
    fsb.Make_Rpc_Server(FLAGS_listen_port, FLAGS_rpc_timeout, FLAGS_rpc_threads);
    fsb.Make_Registry_Object(FLAGS_registry_host, FLAGS_base_service + FLAGS_instance_name, FLAGS_access_host);

    auto server = fsb.Build();
    server->Start();

    return 0;
}