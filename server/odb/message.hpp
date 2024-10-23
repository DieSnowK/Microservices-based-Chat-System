#pragma once
#include <string>
#include <cstddef>
#include <odb/nullable.hxx>
#include <odb/core.hxx>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace SnowK
{
    #pragma db object table("message")
    class Message
    {
    public:
        Message() {}
        Message(const std::string &mid,
                const std::string &ssid,
                const std::string &uid,
                const unsigned char mtype,
                const boost::posix_time::ptime &ctime) 
            : _message_id(mid), _session_id(ssid)
            , _user_id(uid), _message_type(mtype)
            , _create_time(ctime) 
        {}

        std::string Message_Id() const 
        { 
            return _message_id; 
        }
        void Message_Id(const std::string &val) 
        { 
            _message_id = val; 
        }

        std::string Session_Id() const 
        { 
            return _session_id; 
        }
        void Session_Id(const std::string &val) 
        { 
            _session_id = val; 
        }

        std::string User_Id() const 
        { 
            return _user_id; 
        }
        void User_Id(const std::string &val) 
        { 
            _user_id = val; 
        }

        unsigned char Message_Type() const 
        { 
            return _message_type; 
        }
        void Message_Type(unsigned char val) 
        { 
            _message_type = val; 
        }

        boost::posix_time::ptime Create_Time() const 
        { 
            return _create_time; 
        }
        void Create_Time(const boost::posix_time::ptime &val) 
        { 
            _create_time = val; 
        }

        std::string Content() const
        {
            if (!_content)
            {
                return std::string();
            }

            return *_content;
        }
        void Content(const std::string &val) 
        { 
            _content = val; 
        }

        std::string File_Id() const
        {
            if (!_file_id)
            {
                return std::string();
            }

            return *_file_id;
        }
        void File_Id(const std::string &val) 
        { 
            _file_id = val; 
        }

        std::string File_Name() const
        {
            if (!_file_name)
            {
                return std::string();
            }

            return *_file_name;
        }
        void File_Name(const std::string &val) 
        { 
            _file_name = val; 
        }

        unsigned int File_Size() const
        {
            if (!_file_size)
            {
                return 0;
            }

            return *_file_size;
        }
        void File_Size(unsigned int val) 
        { 
            _file_size = val; 
        }

    private:
        friend class odb::access;
        #pragma db id auto
        unsigned long _id;
        #pragma db type("varchar(64)") index unique
        std::string _message_id;
        #pragma db type("varchar(64)") index
        std::string _session_id;                
        #pragma db type("varchar(64)")
        std::string _user_id;                   
        unsigned char _message_type;            
        #pragma db type("TIMESTAMP")
        boost::posix_time::ptime _create_time;  

        odb::nullable<std::string> _content;
        #pragma db type("varchar(64)")
        odb::nullable<std::string> _file_id;
        #pragma db type("varchar(128)")
        odb::nullable<std::string> _file_name;
        odb::nullable<unsigned int> _file_size;
    };
}

// odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time message.hpp