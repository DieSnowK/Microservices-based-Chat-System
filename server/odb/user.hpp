#pragma once
#include <string>
#include <cstddef>
#include <odb/nullable.hxx>
#include <odb/core.hxx>

namespace SnowK
{
    #pragma db object table("user")
    class User
    {
    public:
        User() {}
        User(const std::string &uid, const std::string &nickname, const std::string &password) 
            : _user_id(uid)
            , _nickname(nickname)
            , _password(password) 
        {}
        User(const std::string &uid, const std::string &phone) 
            : _user_id(uid)
            , _nickname(uid)
            , _phone(phone) 
        {}

        void User_Id(const std::string &val) { _user_id = val; }
        std::string User_Id() { return _user_id; }

        std::string Nickname()
        {
            if (!_nickname)
            {
                return std::string();
            }

            return *_nickname;
        }
        void Nickname(const std::string &val) { _nickname = val; }

        std::string Description()
        {
            if (!_description)
            {
                return std::string();
            }

            return *_description;
        }
        void Description(const std::string &val) { _description = val; }

        std::string Password()
        {
            if (!_password)
            {
                return std::string();
            }

            return *_password;
        }
        void Password(const std::string &val) { _password = val; }

        std::string Phone()
        {
            if (!_phone)
            {
                return std::string();
            }

            return *_phone;
        }
        void Phone(const std::string &val) { _phone = val; }

        std::string Avatar_Id()
        {
            if (!_avatar_id)
            {
                return std::string();
            }

            return *_avatar_id;
        }
        void Avatar_Id(const std::string &val) { _avatar_id = val; }

    private:
        friend class odb::access;
        #pragma db id auto
        unsigned long _id;
        
        #pragma db type("varchar(64)") index unique
        std::string _user_id;

        #pragma db type("varchar(64)") index unique
        odb::nullable<std::string> _nickname;    

        odb::nullable<std::string> _description; 

        #pragma db type("varchar(64)")
        odb::nullable<std::string> _password;       

        #pragma db type("varchar(64)") index unique
        odb::nullable<std::string> _phone; 

        #pragma db type("varchar(64)")
        odb::nullable<std::string> _avatar_id; 
    };
}

// odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time user.hpp