#include "mysql_user.hpp"
#include "user.hpp"
#include "user-odb.hxx"
#include <gflags/gflags.h>

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

void Insert(SnowK::UserTable &user_tb)
{
    auto user1 = std::make_shared<SnowK::User>("uid1", "昵称1", "123456");
    user_tb.Insert(user1);

    auto user2 = std::make_shared<SnowK::User>("uid2", "15566667777");
    user_tb.Insert(user2);
}

void Update_By_Id(SnowK::UserTable &user_tb)
{
    auto user = user_tb.Select_By_Id("uid1");
    user->Description("SnowK~");
    user_tb.Update(user);
}

void Update_By_Phone(SnowK::UserTable &user_tb)
{
    auto user = user_tb.Select_By_Phone("15566667777");
    user->Password("233333");
    user_tb.Update(user);
}

void Update_By_Nickname(SnowK::UserTable &user_tb)
{
    auto user = user_tb.Select_By_Nickname("uid2");
    user->Nickname("DSK");
    user_tb.Update(user);
}

void Select_Users(SnowK::UserTable &user_tb)
{
    std::vector<std::string> id_list = {"uid1", "uid2"};
    auto ret = user_tb.Select_Multi_Users(id_list);
    for (auto &user : ret)
    {
        std::cout << user.Nickname() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    auto db = SnowK::ODBFactory::Create("root", "SnowK8989", "127.0.0.1", "SnowK_Test", "utf8", 0, 1);

    SnowK::UserTable user(db);

    // Insert(user);
    // Update_By_Id(user);
    // Update_By_Phone(user);
    // Update_By_Nickname(user);
    // Select_Users(user);

    return 0;
}