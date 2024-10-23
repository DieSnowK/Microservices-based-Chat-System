#include <sw/redis++/redis.h>
#include <iostream>

namespace SnowK
{
    class Session
    {
    public:
        using ptr = std::shared_ptr<Session>;

        Session(const std::shared_ptr<sw::redis::Redis> &redis_client) 
            : _redis_client(redis_client) 
        {}
        
        void Append(const std::string &ssid, const std::string &uid)
        {
            _redis_client->set(ssid, uid);
        }

        void Remove(const std::string &ssid)
        {
            _redis_client->del(ssid);
        }

        sw::redis::OptionalString Uid(const std::string &ssid)
        {
            return _redis_client->get(ssid);
        }

    private:
        std::shared_ptr<sw::redis::Redis> _redis_client;
    };

    class Status
    {
    public:
        using ptr = std::shared_ptr<Status>;

        Status(const std::shared_ptr<sw::redis::Redis> &redis_client) 
            : _redis_client(redis_client) 
        {}

        void Append(const std::string &uid)
        {
            _redis_client->set(uid, "");
        }

        void Remove(const std::string &uid)
        {
            _redis_client->del(uid);
        }

        bool Exists(const std::string &uid)
        {
            if (_redis_client->get(uid))
            {
                return true;
            }

            return false;
        }

    private:
        std::shared_ptr<sw::redis::Redis> _redis_client;
    };

    class Codes
    {
    public:
        using ptr = std::shared_ptr<Codes>;

        Codes(const std::shared_ptr<sw::redis::Redis> &redis_client) 
            : _redis_client(redis_client) 
        {}

        void Append(const std::string &cid, const std::string &code,
                    const std::chrono::milliseconds &t = std::chrono::milliseconds(300000))
        {
            _redis_client->set(cid, code, t);
        }

        void Remove(const std::string &cid)
        {
            _redis_client->del(cid);
        }
        
        sw::redis::OptionalString Code(const std::string &cid)
        {
            return _redis_client->get(cid);
        }

    private:
        std::shared_ptr<sw::redis::Redis> _redis_client;
    };
}