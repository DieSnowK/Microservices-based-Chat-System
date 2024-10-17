#pragma once
#include "mysql.hpp"
#include "chat_session.hpp"
#include "mysql_chat_session_member.hpp"
#include "chat_session-odb.hxx"

namespace SnowK
{
    class ChatSessionTable
    {
    public:
        using ptr = std::shared_ptr<ChatSessionTable>;

        ChatSessionTable(const std::shared_ptr<odb::core::database> &db) 
            : _db(db) 
        {}
        
        bool Insert(ChatSession &cs)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->persist(cs);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to add a new session {}:{}",
                          cs.Chat_Session_Name(), e.what());
                return false;
            }

            return true;
        }

        bool Remove(const std::string &ssid)
        {
            typedef odb::query<ChatSession> query;
            typedef odb::query<ChatSessionMember> mquery;
            try
            {
                odb::transaction trans(_db->begin());
                _db->erase_query<ChatSession>(query::chat_session_id == ssid);
                _db->erase_query<ChatSessionMember>(mquery::session_id == ssid);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to delete the session {}:{}", ssid, e.what());
                return false;
            }
            return true;
        }

        // Deletion of a one-on-one session,
        // -- based on the two members of the one-on-one session
        bool Remove(const std::string &uid, const std::string &pid)
        {
            typedef odb::query<SingleChatSession> query;
            typedef odb::query<ChatSession> cquery;
            typedef odb::query<ChatSessionMember> mquery;
            try
            {
                odb::transaction trans(_db->begin());

                auto ret = _db->query_one<SingleChatSession>(
                    query::csm1::user_id == uid &&
                    query::csm2::user_id == pid &&
                    query::css::chat_session_type == ChatSessionType::SINGLE);

                std::string cssid = ret->chat_session_id;
                _db->erase_query<ChatSession>(cquery::chat_session_id == cssid);
                _db->erase_query<ChatSessionMember>(mquery::session_id == cssid);

                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to delete the session {}-{}:{}", uid, pid, e.what());
                return false;
            }

            return true;
        }

        std::shared_ptr<ChatSession> Select(const std::string &ssid)
        {
            std::shared_ptr<ChatSession> ret;
            typedef odb::query<ChatSession> query;
            try
            {
                odb::transaction trans(_db->begin());
                ret.reset(_db->query_one<ChatSession>(query::chat_session_id == ssid));
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to obtain session information by session ID {}:{}", 
                          ssid, e.what());
            }

            return ret;
        }

        std::vector<SingleChatSession> SingleChatSessions(const std::string &uid)
        {
            std::vector<SingleChatSession> ret;
            typedef odb::query<SingleChatSession> query;
            typedef odb::result<SingleChatSession> result;
            try
            {
                odb::transaction trans(_db->begin());

                // TODO Why?
                result r(_db->query<SingleChatSession>(
                    query::css::chat_session_type == ChatSessionType::SINGLE &&
                    query::csm1::user_id == uid &&
                    query::csm2::user_id != query::csm1::user_id));
                for (auto i(r.begin()); i != r.end(); ++i)
                {
                    ret.push_back(*i);
                }

                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to get a {}'s one-to-one chat session: {}", 
                          uid, e.what());
            }

            return ret;
        }

        std::vector<GroupChatSession> GroupChatSessions(const std::string &uid)
        {
            std::vector<GroupChatSession> ret;
            typedef odb::query<GroupChatSession> query;
            typedef odb::result<GroupChatSession> result;
            try
            {
                odb::transaction trans(_db->begin());
                
                result r(_db->query<GroupChatSession>(
                    query::css::chat_session_type == ChatSessionType::GROUP &&
                    query::csm::user_id == uid));
                for (auto i(r.begin()); i != r.end(); ++i)
                {
                    ret.push_back(*i);
                }

                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to get the group chat session for user {} : {}", 
                          uid, e.what());
            }
            
            return ret;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}