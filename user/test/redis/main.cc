#include <gflags/gflags.h>
#include <thread>
#include "data_redis.hpp"
#include "redis.hpp"

DEFINE_bool(run_mode, false, "���������ģʽ��false-���ԣ� true-������");
DEFINE_string(log_file, "", "����ģʽ�£�����ָ����־������ļ�");
DEFINE_int32(log_level, 0, "����ģʽ�£�����ָ����־����ȼ�");

DEFINE_string(ip, "127.0.0.1", "���Ƿ�������IP��ַ����ʽ��127.0.0.1");
DEFINE_int32(port, 6379, "���Ƿ������Ķ˿�, ��ʽ: 8080");
DEFINE_int32(db, 0, "��ı�ţ�Ĭ��0��");
DEFINE_bool(keep_alive, true, "�Ƿ���г����ӱ���");

void session_test(const std::shared_ptr<sw::redis::Redis> &client)
{
    bite_im::Session ss(client);
    ss.append("�ỰID1", "�û�ID1");
    ss.append("�ỰID2", "�û�ID2");
    ss.append("�ỰID3", "�û�ID3");
    ss.append("�ỰID4", "�û�ID4");

    ss.remove("�ỰID2");
    ss.remove("�ỰID3");

    auto res1 = ss.uid("�ỰID1");
    if (res1)
        std::cout << *res1 << std::endl;
    auto res2 = ss.uid("�ỰID2");
    if (res2)
        std::cout << *res2 << std::endl;
    auto res3 = ss.uid("�ỰID3");
    if (res3)
        std::cout << *res3 << std::endl;
    auto res4 = ss.uid("�ỰID4");
    if (res4)
        std::cout << *res4 << std::endl;
}

void status_test(const std::shared_ptr<sw::redis::Redis> &client)
{
    bite_im::Status status(client);
    status.append("�û�ID1");
    status.append("�û�ID2");
    status.append("�û�ID3");

    status.remove("�û�ID2");

    if (status.exists("�û�ID1"))
        std::cout << "�û�1���ߣ�" << std::endl;
    if (status.exists("�û�ID2"))
        std::cout << "�û�2���ߣ�" << std::endl;
    if (status.exists("�û�ID3"))
        std::cout << "�û�3���ߣ�" << std::endl;
}

void code_test(const std::shared_ptr<sw::redis::Redis> &client)
{
    bite_im::Codes codes(client);
    codes.append("��֤��ID1", "��֤��1");
    codes.append("��֤��ID2", "��֤��2");
    codes.append("��֤��ID3", "��֤��3");

    codes.remove("��֤��ID2");

    auto y1 = codes.code("��֤��ID1");
    auto y2 = codes.code("��֤��ID2");
    auto y3 = codes.code("��֤��ID3");
    if (y1)
        std::cout << *y1 << std::endl;
    if (y2)
        std::cout << *y2 << std::endl;
    if (y3)
        std::cout << *y3 << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(4));
    auto y4 = codes.code("��֤��ID1");
    auto y5 = codes.code("��֤��ID2");
    auto y6 = codes.code("��֤��ID3");
    if (!y4)
        std::cout << "��֤��ID1������" << std::endl;
    if (!y5)
        std::cout << "��֤��ID2������" << std::endl;
    if (!y6)
        std::cout << "��֤��ID3������" << std::endl;
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    // bite_im::init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);

    auto client = SnowK::RedisClientFactory::Create(FLAGS_ip, FLAGS_port, FLAGS_db, FLAGS_keep_alive);

    // session_test(client);
    // status_test(client);
    code_test(client);
    return 0;
}
