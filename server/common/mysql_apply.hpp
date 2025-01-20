#pragma once
#include "mysql.hpp"
#include "friend_apply.hpp"
#include "friend_apply-odb.hxx"

namespace SnowK
{
    class FriendApplyTable
    {
        using query = odb::query<FriendApply>;
        using result = odb::result<FriendApply>;

    public:
        using ptr = std::shared_ptr<FriendApplyTable>;
        
        FriendApplyTable(const std::shared_ptr<odb::core::database> &db) 
            : _db(db) 
        {}

        bool Insert(FriendApply &ev)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->persist(ev);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("The application for a new friend event failed {} - {}: {}",
                          ev.User_Id(), ev.Peer_Id(), e.what());
                return false;
            }

            return true;
        }

        // TODO flag can be cancled ?
        bool Exists(const std::string &uid, const std::string &pid)
        {
            bool flag = false;
            try
            {
                odb::transaction trans(_db->begin());
                result r(_db->query<FriendApply>(query::user_id == uid && query::peer_id == pid));
                flag = !r.empty();
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Faild to check whether the friend application event:{} - {}: {}", uid, pid, e.what());
            }

            return flag;
        }

        bool Remove(const std::string &uid, const std::string &pid)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->erase_query<FriendApply>(query::user_id == uid && query::peer_id == pid);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to delete a friend request{} - {}: {}", uid, pid, e.what());
                return false;
            }

            return true;
        }

        // Obtain the ID of all friend requesters of the currently specified user
        std::vector<std::string> ApplyUsers(const std::string &uid)
        {
            std::vector<std::string> ret;
            try
            {
                odb::transaction trans(_db->begin());

                result r(_db->query<FriendApply>(query::peer_id == uid));
                for (auto iter(r.begin()); iter != r.end(); ++iter)
                {
                    ret.push_back(iter->User_Id());
                }

                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to get the friend applicant list of user {}: {}", uid, e.what());
            }

            return ret;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}