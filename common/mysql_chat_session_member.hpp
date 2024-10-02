#pragma once
#include "mysql.hpp"
#include "chat_session_member.hpp"
#include "chat_session_member-odb.hxx"

namespace SnowK
{
    class ChatSessionMemeberTable
    {
    public:
        using ptr = std::shared_ptr<ChatSessionMemeberTable>;

        ChatSessionMemeberTable(const std::shared_ptr<odb::core::database> &db) 
            : _db(db) 
        {}

        // Add single member
        bool Append(ChatSessionMember &csm)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->persist(csm);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to add a single session member {}-{}:{}",
                          csm.Session_Id(), csm.User_Id(), e.what());
                return false;
            }

            return true;
        }

        // Add multiple members
        bool Append(std::vector<ChatSessionMember> &csm_lists)
        {
            try
            {
                odb::transaction trans(_db->begin());
                for (auto &csm : csm_lists)
                {
                    _db->persist(csm);
                }
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to add multiple session members {}-{}:{}",
                          csm_lists[0].Session_Id(), csm_lists.size(), e.what());
                return false;
            }

            return true;
        }

        // Delete a specified member in a specified session
        bool Remove(ChatSessionMember &csm)
        {
            typedef odb::query<ChatSessionMember> query;
            try
            {
                odb::transaction trans(_db->begin());
                _db->erase_query<ChatSessionMember>(query::session_id == csm.Session_Id() &&
                                                    query::user_id == csm.User_Id());
                trans.commit();
            }
            catch (std::exception &e)
            {
                LOG_ERROR("Failed to remove a single session member {}-{}:{}",
                          csm.Session_Id(), csm.User_Id(), e.what());
                return false;
            }

            return true;
        }

        bool Remove(const std::string &ssid)
        {
            typedef odb::query<ChatSessionMember> query;
            try
            {
                odb::transaction trans(_db->begin());
                _db->erase_query<ChatSessionMember>(query::session_id == ssid);
                trans.commit();
            }
            catch (std::exception &e)
            {
                LOG_ERROR("Failed to remove a single session member {}:{}", ssid, e.what());
                return false;
            }

            return true;
        }

        std::vector<std::string> Members(const std::string &ssid)
        {
            typedef odb::query<ChatSessionMember> query;
            typedef odb::result<ChatSessionMember> result;
            std::vector<std::string> ret;
            try
            {
                odb::transaction trans(_db->begin());
                result r(_db->query<ChatSessionMember>(query::session_id == ssid));
                for (auto i(r.begin()); i != r.end(); ++i)
                {
                    ret.push_back(i->User_Id());
                }
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to get session members: {}-{}", ssid, e.what());
            }

            return ret;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}