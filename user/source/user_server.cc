#include "user_server.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

DEFINE_string(registry_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(access_host, "127.0.0.1:10003", "The external access address of the current instance");

DEFINE_int32(listen_port, 10003, "The RPC server listens on the port");
DEFINE_int32(rpc_timeout, -1, "RPC call timeout period");
DEFINE_int32(rpc_threads, 1, "The number of I/O threads of the rpc");

DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(instance_name, "/user_service/instance", "Name of the current instance");
DEFINE_string(file_service, "/service/file_service", "The name of the file management subservice");

DEFINE_string(es_host, "http://127.0.0.1:9200/", "ES search engine server URL");

DEFINE_string(mysql_host, "127.0.0.1", "MySQL access address");
DEFINE_int32(mysql_port, 0, "Access port of the MySQL server");
DEFINE_string(mysql_user, "root", "MySQL server access username");
DEFINE_string(mysql_pwd, "SnowK8989", "MySQL server access password");
DEFINE_string(mysql_db, "SnowK_Test", "The default name of the MySQL database");
DEFINE_string(mysql_cset, "utf8", "MySQL client character set");
DEFINE_int32(mysql_pool_count, 4, "The maximum number of connections in a MySQL connection pool");

DEFINE_string(redis_host, "127.0.0.1", "Redis access address");
DEFINE_int32(redis_port, 6379, "Access port of the Redis server");
DEFINE_int32(redis_db, 0, "The default name of the Redis database");
DEFINE_bool(redis_keep_alive, true, "Redis persistent keep-alive option");

DEFINE_string(dms_key_id, "", "SMS platform key ID");
DEFINE_string(dms_key_secret, "", "SMS platform key");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    SnowK::UserServerBuilder usb;
    usb.make_dms_object(FLAGS_dms_key_id, FLAGS_dms_key_secret);
    usb.make_es_object({FLAGS_es_host});
    usb.make_mysql_object(FLAGS_mysql_user, FLAGS_mysql_pwd, FLAGS_mysql_host,
                          FLAGS_mysql_db, FLAGS_mysql_cset, FLAGS_mysql_port, FLAGS_mysql_pool_count);
    usb.make_redis_object(FLAGS_redis_host, FLAGS_redis_port, FLAGS_redis_db, FLAGS_redis_keep_alive);
    usb.make_discovery_object(FLAGS_registry_host, FLAGS_base_service, FLAGS_file_service);
    usb.make_rpc_server(FLAGS_listen_port, FLAGS_rpc_timeout, FLAGS_rpc_threads);
    usb.make_registry_object(FLAGS_registry_host, FLAGS_base_service + FLAGS_instance_name, FLAGS_access_host);

    auto server = usb.Build();
    server->Start();

    return 0;
}