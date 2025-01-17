#pragma once
#include <string>
#include <cstddef>
#include <odb/core.hxx>

namespace SnowK
{
        #pragma db object table("friend_apply")
    class FriendApply
    {
    public:
        FriendApply() {}
        FriendApply(const std::string &eid,
                    const std::string &uid, const std::string &pid) 
            : _user_id(uid)
            , _peer_id(pid)
            , _event_id(eid)
        {}

        std::string Event_Id() const 
        { 
            return _event_id; 
        }
        void Event_Id(std::string &eid) 
        { 
            _event_id = eid; 
        }

        std::string User_Id() const 
        { 
            return _user_id; 
        }
        void User_Id(std::string &uid) 
        { 
            _user_id = uid; 
        }

        std::string Peer_Id() const 
        { 
            return _peer_id; 
        }
        void Peer_Id(std::string &uid) 
        { 
            _peer_id = uid; 
        }

    private:
        friend class odb::access;
        #pragma db id auto
        unsigned long _id;
        #pragma db type("varchar(64)") index unique
        std::string _event_id;
        #pragma db type("varchar(64)") index
        std::string _user_id;
        #pragma db type("varchar(64)") index
        std::string _peer_id;
    };
}

// odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time friend_apply.hpp