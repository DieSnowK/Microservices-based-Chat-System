#include "message_server.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");


DEFINE_string(registry_host, "http://127.0.0.1:2379", "Service registry address");
DEFINE_string(access_host, "127.0.0.1:10005", "The external access address of the current instance");

DEFINE_int32(listen_port, 10005, "The RPC server listens on the port");
DEFINE_int32(rpc_timeout, -1, "RPC call timeout period");
DEFINE_int32(rpc_threads, 1, "The number of I/O threads of the rpc");

DEFINE_string(instance_name, "/message_service/instance", "Name of the current instance");
DEFINE_string(base_service, "/service", "The root directory of service monitoring");
DEFINE_string(file_service, "/service/file_service", "The name of the user management subservice");
DEFINE_string(user_service, "/service/user_service", "The name of the file management subservice");

DEFINE_string(es_host, "http://127.0.0.1:9200/", "ES search engine server URL");

DEFINE_string(mysql_host, "127.0.0.1", "MySQL access address");
DEFINE_int32(mysql_port, 0, "Access port of the MySQL server");
DEFINE_string(mysql_user, "root", "MySQL server access username");
DEFINE_string(mysql_pwd, "SnowK8989", "MySQL server access password");
DEFINE_string(mysql_db, "SnowK_Test", "The default name of the MySQL database");
DEFINE_string(mysql_cset, "utf8", "MySQL client character set");
DEFINE_int32(mysql_pool_count, 4, "The maximum number of connections in a MySQL connection pool");

DEFINE_string(mq_user, "root", "The Message Queuing server access username");
DEFINE_string(mq_pswd, "SnowK8989", "Message Queuing server access password");
DEFINE_string(mq_host, "127.0.0.1:5672", "The address of the Message Queuing server");
DEFINE_string(mq_msg_exchange, "msg_exchange", "The name of the issuing switch for the persistent message");
DEFINE_string(mq_msg_queue, "msg_queue", "The name of the publishing queue for the persistence message");
DEFINE_string(mq_msg_binding_key, "msg_queue", "Message binding key"); 

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    SnowK::MessageServerBuilder msb;
    msb.Make_Mq_Object(FLAGS_mq_user, FLAGS_mq_pswd, FLAGS_mq_host,
                       FLAGS_mq_msg_exchange, FLAGS_mq_msg_queue, FLAGS_mq_msg_binding_key);
    msb.Make_Es_Object({FLAGS_es_host});
    msb.Make_MySQL_Object(FLAGS_mysql_user, FLAGS_mysql_pwd, FLAGS_mysql_host,
                          FLAGS_mysql_db, FLAGS_mysql_cset, FLAGS_mysql_port, FLAGS_mysql_pool_count);
    msb.Make_Discovery_Object(FLAGS_registry_host, FLAGS_base_service, FLAGS_file_service, FLAGS_user_service);
    msb.Make_Rpc_Server(FLAGS_listen_port, FLAGS_rpc_timeout, FLAGS_rpc_threads);
    msb.Make_Registry_Object(FLAGS_registry_host, FLAGS_base_service + FLAGS_instance_name, FLAGS_access_host);
    
    auto server = msb.Build();
    server->Start();
    
    return 0;
}