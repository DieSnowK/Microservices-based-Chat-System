#pragma once
#include <string>
#include <cstddef>
#include <odb/core.hxx>

namespace SnowK
{
    #pragma db object table("chat_session_member")
    class ChatSessionMember
    {
    public:
        ChatSessionMember() {}
        ChatSessionMember(const std::string &ssid, const std::string &uid) 
            : _session_id(ssid)
            , _user_id(uid) 
        {}
        ~ChatSessionMember() {}

        std::string Session_Id() const 
        { 
            return _session_id; 
        }
        void Session_Id(std::string &ssid) 
        { 
            _session_id = ssid; 
        }

        std::string User_Id() const 
        { 
            return _user_id; 
        }
        void User_Id(std::string &uid)
        { 
            _user_id = uid; 
        }

    private:
        friend class odb::access;
    #pragma db id auto
        unsigned long _id;
    #pragma db type("varchar(64)") index
        std::string _session_id;
    #pragma db type("varchar(64)")
        std::string _user_id;
    };
}