#include <gflags/gflags.h>
#include "elasticsearch.hpp"

DEFINE_bool(mode, false, "true: Release, false: Debug");
DEFINE_string(log_file, "", "In Release, specify the output file of the log");
DEFINE_int32(log_level, 0, "In Release, specify the log output level");

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    SnowK::InitLogger(FLAGS_mode, FLAGS_log_file, FLAGS_log_level);

    std::vector<std::string> host_list({"http://127.0.0.1:9200/"});
    auto client = std::make_shared<elasticlient::Client>(host_list);

    // 1.Create Index
    bool ret = SnowK::ESIndex(client, "test_user")
                   .Append_Properties("nickname")
                   .Append_Properties("phone", "keyword", "standard", true)
                   .Create();
    if(ret == false)
    {
        LOG_ERROR("Failed to create index");
        return -1;
    }
    else
    {
        LOG_INFO("The index is created");
    }

    // 2.Insert Data
    ret = SnowK::ESInsert(client, "test_user")
              .Append_Item("nickname", "WSL")
              .Append_Item("phone", "233333333")
              .Insert("0001");
    if(ret == false)
    {
        LOG_ERROR("Failed to insert data");
        return -1;
    }
    else
    {
        LOG_INFO("The data has been added successfully");
    }

    // 3.Modify Data
    ret = SnowK::ESInsert(client, "test_user")
              .Append_Item("nickname", "SnowK")
              .Append_Item("phone", "66666666")
              .Insert("0001");
    if (ret == false)
    {
        LOG_ERROR("Failed to modify data");
        return -1;
    }
    else
    {
        LOG_INFO("The data was updated successfully!");
    }

    // 4.Search Data
    Json::Value user = SnowK::ESSearch(client, "test_user")
                           .Append_Should_Match("phone.keyword", "66666666")
                           //.Append_Must_Not_Terms("nickname.keyword", {"SnowK"})
                           .Search();
    if (user.empty() || user.isArray() == false)
    {
        LOG_ERROR("The result is empty or the result is not of array type");
        return -1;
    }
    else
    {
        LOG_INFO("The data search was successful");
    }

    int sz = user.size();
    LOG_INFO("Number of search result entries: {}", sz);
    for (int i = 0; i < sz; i++)
    {
        LOG_INFO("nickname: {}", user[i]["_source"]["nickname"].asString());
    }

    // 5.Delete Data
    ret = SnowK::ESRemove(client, "test_user").Remove("0001");
    if (ret == false)
    {
        LOG_ERROR("Failed to delete data");
        return -1;
    }
    else
    {
        LOG_INFO("The data was deleted successfully!");
    }

    return 0;
}