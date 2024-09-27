#include "elasticsearch.hpp"
#include "user.hpp"
// #include "message.hpp"

namespace SnowK
{
    class ESUser
    {
    public:
        using ptr = std::shared_ptr<ESUser>;

        ESUser(const std::shared_ptr<elasticlient::Client> &client) 
            : _es_client(client) 
        {}

        bool CreateIndex()
        {
            bool ret = ESIndex(_es_client, "user")
                           .Append_Properties("user_id", "keyword", "standard", true)
                           .Append_Properties("nickname")
                           .Append_Properties("phone", "keyword", "standard", true)
                           .Append_Properties("description", "text", "standard", false)
                           .Append_Properties("avatar_id", "keyword", "standard", false)
                           .Create();
            if (ret == false)
            {
                LOG_ERROR("Failed to create a user information index");
                return false;
            }

            LOG_INFO("The user information index is created");
            return true;
        }

        bool AppendData(const std::string &uid,
                        const std::string &phone,
                        const std::string &nickname,
                        const std::string &description,
                        const std::string &avatar_id)
        {
            bool ret = ESInsert(_es_client, "user")
                           .Append_Item("user_id", uid)
                           .Append_Item("nickname", nickname)
                           .Append_Item("phone", phone)
                           .Append_Item("description", description)
                           .Append_Item("avatar_id", avatar_id)
                           .Insert(uid);
            if (ret == false)
            {
                LOG_ERROR("Failed to insert/update user data");
                return false;
            }

            LOG_INFO("User data is added/updated successfully");
            return true;
        }

        std::vector<User> Search(const std::string &key, const std::vector<std::string> &uid_list)
        {
            std::vector<User> ret;
            Json::Value json_user = ESSearch(_es_client, "user")
                                        .Append_Should_Match("phone.keyword", key)
                                        .Append_Should_Match("user_id.keyword", key)
                                        .Append_Should_Match("nickname", key)
                                        .Append_Must_Not_Terms("user_id.keyword", uid_list)
                                        .Search();
            if (json_user.isArray() == false)
            {
                LOG_ERROR("The user search result is not of array type");
                return ret;
            }

            int sz = json_user.size();
            LOG_DEBUG("Number of search result entries: {}", sz);
            
            for (int i = 0; i < sz; i++)
            {
                User user;
                user.User_Id(json_user[i]["_source"]["user_id"].asString());
                user.Nickname(json_user[i]["_source"]["nickname"].asString());
                user.Description(json_user[i]["_source"]["description"].asString());
                user.Phone(json_user[i]["_source"]["phone"].asString());
                user.Avatar_Id(json_user[i]["_source"]["avatar_id"].asString());
                ret.push_back(user);
            }
            
            return ret;
        }

    private:
        std::shared_ptr<elasticlient::Client> _es_client;
    };

//     class ESMessage
//     {
//     public:
//         using ptr = std::shared_ptr<ESMessage>;
//         ESMessage(const std::shared_ptr<elasticlient::Client> &es_client) : _es_client(es_client) {}
//         bool createIndex()
//         {
//             bool ret = ESIndex(_es_client, "message")
//                            .Append("user_id", "keyword", "standard", false)
//                            .Append("message_id", "keyword", "standard", false)
//                            .Append("create_time", "long", "standard", false)
//                            .Append("chat_session_id", "keyword", "standard", true)
//                            .Append("content")
//                            .create();
//             if (ret == false)
//             {
//                 LOG_INFO("消息信息索引创建失败!");
//                 return false;
//             }
//             LOG_INFO("消息信息索引创建成功!");
//             return true;
//         }
//         bool AppendData(const std::string &user_id,
//                         const std::string &message_id,
//                         const long create_time,
//                         const std::string &chat_session_id,
//                         const std::string &content)
//         {
//             bool ret = ESInsert(_es_client, "message")
//                            .Append("message_id", message_id)
//                            .Append("create_time", create_time)
//                            .Append("user_id", user_id)
//                            .Append("chat_session_id", chat_session_id)
//                            .Append("content", content)
//                            .insert(message_id);
//             if (ret == false)
//             {
//                 LOG_ERROR("消息数据插入/更新失败!");
//                 return false;
//             }
//             LOG_INFO("消息数据新增/更新成功!");
//             return true;
//         }
//         bool remove(const std::string &mid)
//         {
//             bool ret = ESRemove(_es_client, "message").remove(mid);
//             if (ret == false)
//             {
//                 LOG_ERROR("消息数据删除失败!");
//                 return false;
//             }
//             LOG_INFO("消息数据删除成功!");
//             return true;
//         }
//         std::vector<SnowK::Message> search(const std::string &key, const std::string &ssid)
//         {
//             std::vector<SnowK::Message> res;
//             Json::Value json_user = ESSearch(_es_client, "message")
//                                         .Append_must_term("chat_session_id.keyword", ssid)
//                                         .Append_must_match("content", key)
//                                         .search();
//             if (json_user.isArray() == false)
//             {
//                 LOG_ERROR("用户搜索结果为空，或者结果不是数组类型");
//                 return res;
//             }
//             int sz = json_user.size();
//             LOG_DEBUG("检索结果条目数量：{}", sz);
//             for (int i = 0; i < sz; i++)
//             {
//                 SnowK::Message message;
//                 message.user_id(json_user[i]["_source"]["user_id"].asString());
//                 message.message_id(json_user[i]["_source"]["message_id"].asString());
//                 boost::posix_time::ptime ctime(boost::posix_time::from_time_t(
//                     json_user[i]["_source"]["create_time"].asInt64()));
//                 message.create_time(ctime);
//                 message.session_id(json_user[i]["_source"]["chat_session_id"].asString());
//                 message.content(json_user[i]["_source"]["content"].asString());
//                 res.push_back(message);
//             }
//             return res;
//         }

//     private:
//         std::shared_ptr<elasticlient::Client> _es_client;
//     };
}