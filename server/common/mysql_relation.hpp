#pragma once
#include "mysql.hpp"
#include "relation.hpp"
#include "relation-odb.hxx"

namespace SnowK
{
    class RelationTable
    {
        using query = odb::query<Relation>;
        using result = odb::result<Relation>;

    public:
        using ptr = std::shared_ptr<RelationTable>;

        RelationTable(const std::shared_ptr<odb::core::database> &db) 
            : _db(db) 
        {}
        
        bool Insert(const std::string &uid, const std::string &pid)
        {
            try
            {
                Relation r1(uid, pid);
                Relation r2(pid, uid);

                odb::transaction trans(_db->begin());
                _db->persist(r1);
                _db->persist(r2);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to add the user's friend relationship information {} - {}: {}", 
                          uid, pid, e.what());
                return false;
            }

            return true;
        }

        bool Remove(const std::string &uid, const std::string &pid)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->erase_query<Relation>(query::user_id == uid && query::peer_id == pid);
                _db->erase_query<Relation>(query::user_id == pid && query::peer_id == uid);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to delete friend relationship information {} - {}: {}",
                          uid, pid, e.what());
                return false;
            }

            return true;
        }

        // TODO flag can be cancled?
        bool Exists(const std::string &uid, const std::string &pid)
        {
            result ret;
            bool flag = false;
            try
            {
                odb::transaction trans(_db->begin());
                ret = _db->query<Relation>(query::user_id == uid && query::peer_id == pid);
                flag = !ret.empty();
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to get the user's friend relationship:{} - {}: {}", uid, pid, e.what());
            }

            return flag;
        }

        std::vector<std::string> Friends(const std::string &uid)
        {
            std::vector<std::string> ret;
            try
            {
                odb::transaction trans(_db->begin());
                
                result r(_db->query<Relation>(query::user_id == uid));
                for (auto iter(r.begin()); iter != r.end(); ++iter)
                {
                    ret.push_back(iter->Peer_Id());
                }

                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to get all friend IDs of user {}: {}", uid, e.what());
            }

            return ret;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}