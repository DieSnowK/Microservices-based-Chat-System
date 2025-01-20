#pragma once
#include <sw/redis++/redis.h>

namespace SnowK
{
    // Factory Pattern
    class RedisClientFactory
    {
    public:
        static std::shared_ptr<sw::redis::Redis> Create(const std::string &host,
                                                        int port,
                                                        int db,
                                                        bool keep_alive)
        {
            sw::redis::ConnectionOptions opts;
            opts.host = host;
            opts.port = port;
            opts.db = db;
            opts.keep_alive = keep_alive;

            return std::make_shared<sw::redis::Redis>(opts);
        }
    };
}