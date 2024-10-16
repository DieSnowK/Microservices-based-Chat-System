#pragma once
#include <string>
#include <cstddef>
#include <odb/nullable.hxx>
#include <odb/core.hxx>
#include "chat_session_member.hpp"

namespace SnowK
{

    enum class ChatSessionType
    {
        SINGLE = 1,
        GROUP = 2
    };

    #pragma db object table("chat_session")
    class ChatSession
    {
    public:
        ChatSession() {}
        ChatSession(const std::string &ssid,
                    const std::string &ssname, const ChatSessionType sstype) 
            : _chat_session_id(ssid)
            , _chat_session_name(ssname)
            , _chat_session_type(sstype) 
        {}

        std::string Chat_Session_Id() const 
        { 
            return _chat_session_id; 
        }
        void Chat_Session_Id(std::string &ssid)
        { 
            _chat_session_id = ssid; 
        }

        std::string Chat_Session_Name() const 
        { 
            return _chat_session_name; 
        }
        void Chat_Session_Name(std::string &ssname) 
        { 
            _chat_session_name = ssname; 
        }

        ChatSessionType Chat_Session_Type() const 
        { 
            return _chat_session_type; 
        }
        void Chat_Session_Type(ChatSessionType val)
        { 
            _chat_session_type = val; 
        }

    private:
        friend class odb::access;
    #pragma db id auto
        unsigned long _id;
    #pragma db type("varchar(64)") index unique
        std::string _chat_session_id;
    #pragma db type("varchar(64)")
        std::string _chat_session_name;
    #pragma db type("tinyint")
        ChatSessionType _chat_session_type;
    };

    // TODO
    // condition:  css::chat_session_type==1 && csm1.user_id=uid && csm2.user_id != csm1.user_id
    #pragma db view object(ChatSession = css)                                             \
            object(ChatSessionMember = csm1 : css::_chat_session_id == csm1::_session_id) \
            object(ChatSessionMember = csm2 : css::_chat_session_id == csm3::_session_id) \
            query((?))
    struct SingleChatSession
    {
        #pragma db column(css::_chat_session_id)
        std::string chat_session_id;
        #pragma db column(csm2::_user_id)
        std::string friend_id;
    };

    // TODO
    // condition  css::chat_session_type==2 && csm.user_id=uid
    #pragma db view object(ChatSession = css)                                           \
            object(ChatSessionMember = csm : css::_chat_session_id == csm::_session_id) \
            query((?))
    struct GroupChatSession
    {
        #pragma db column(css::_chat_session_id)
        std::string chat_session_id;
        #pragma db column(css::_chat_session_name)
        std::string chat_session_name;
    };
}

// odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time chat_session.hpp