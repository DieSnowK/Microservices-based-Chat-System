#include "mysql.hpp"
#include "user.hpp"
#include "user-odb.hxx"

namespace SnowK
{
    class UserTable
    {
    public:
        using ptr = std::shared_ptr<UserTable>;

        UserTable(const std::shared_ptr<odb::core::database> &db) 
            : _db(db) 
        {}

        bool Insert(const std::shared_ptr<User> &user)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->persist(*user);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to add user {}: {}", user->Nickname(), e.what());
                return false;
            }

            return true;
        }

        bool Update(const std::shared_ptr<User> &user)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->update(*user);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to update user {}: {}", user->Nickname(), e.what());
                return false;
            }

            return true;
        }

        std::shared_ptr<User> Select_By_Nickname(const std::string &nickname)
        {
            std::shared_ptr<User> ret;

            try
            {
                odb::transaction trans(_db->begin());
                ret.reset(_db->query_one<User>(odb::query<User>::nickname == nickname));
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to query a user by nickname {}: {}", nickname, e.what());
            }

            return ret;
        }

        std::shared_ptr<User> Select_By_Phone(const std::string &phone)
        {
            std::shared_ptr<User> ret;

            try
            {
                odb::transaction trans(_db->begin());
                ret.reset(_db->query_one<User>(odb::query<User>::phone == phone));
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to query a user by phone {}: {}", phone, e.what());
            }

            return ret;
        }

        std::shared_ptr<User> Select_By_Id(const std::string &user_id)
        {
            std::shared_ptr<User> ret;

            try
            {
                odb::transaction trans(_db->begin());
                ret.reset(_db->query_one<User>(odb::query<User>::user_id == user_id));
                trans.commit();
            }
            catch (std::exception &e)
            {
                LOG_ERROR("Failed to query a user by user_id {}: {}", user_id, e.what());
            }

            return ret;
        }

        std::vector<User> Select_Multi_Users(const std::vector<std::string> &id_list)
        {
            if (id_list.empty())
            {
                return std::vector<User>();
            }

            // select * from user where id in ('id1', 'id2', ...)
            std::vector<User> ret;
            try
            {
                odb::transaction trans(_db->begin());

                // TODO Could it be more elegant?
                std::stringstream ss;
                ss << "user_id in (";
                for (const auto &id : id_list)
                {
                    ss << "'" << id << "',";
                }
                std::string condition = ss.str();
                condition.pop_back();
                condition += ")";

                odb::result<User> r(_db->query<User>(condition));
                for (auto i(r.begin()); i != r.end(); ++i)
                {
                    ret.push_back(*i);
                }

                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to query users by user ID in batches: {}", e.what());
            }

            return ret;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}