#pragma once
#include "mysql.hpp"
#include "relation.hpp"
#include "relation-odb.hxx"

namespace SnowK
{
    class RelationTable
    {
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
                LOG_ERROR("Failed to add the user's friend relationship information {}-{}:{}", 
                          uid, pid, e.what());
                return false;
            }

            return true;
        }

        bool Remove(const std::string &uid, const std::string &pid)
        {
            typedef odb::query<Relation> query;
            try
            {
                odb::transaction trans(_db->begin());
                _db->erase_query<Relation>(query::user_id == uid && query::peer_id == pid);
                _db->erase_query<Relation>(query::user_id == pid && query::peer_id == uid);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to delete friend relationship information {}-{}:{}",
                          uid, pid, e.what());
                return false;
            }

            return true;
        }

        bool Exists(const std::string &uid, const std::string &pid)
        {
            typedef odb::query<Relation> query;
            typedef odb::result<Relation> result;
            result r;
            bool flag = false;
            try
            {
                odb::transaction trans(_db->begin());
                r = _db->query<Relation>(query::user_id == uid && query::peer_id == pid);
                flag = !r.empty();
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to get the user's friend relationship:{}-{}-{}",
                          uid, pid, e.what());
            }

            return flag;
        }

        std::vector<std::string> Friends(const std::string &uid)
        {
            typedef odb::query<Relation> query;
            typedef odb::result<Relation> result;
            std::vector<std::string> ret;
            try
            {
                odb::transaction trans(_db->begin());
                
                result r(_db->query<Relation>(query::user_id == uid));
                for (auto i(r.begin()); i != r.end(); ++i)
                {
                    ret.push_back(i->peer_id());
                }

                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to get all friend IDs of user {}", uid, e.what());
            }

            return ret;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}