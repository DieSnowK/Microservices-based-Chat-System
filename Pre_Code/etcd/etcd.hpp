#pragma once
#include <functional>
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Value.hpp>
#include "logger.hpp"

namespace SnowK
{
    class Registry
    {
    public:
        using ptr = std::shared_ptr<Registry>;

        Registry(const std::string &host)
            : _client(std::make_shared<etcd::Client>(host))
            , _keep_alive(_client->leasekeepalive(3).get())
            , _lease_id(_keep_alive->Lease())
        {}

        ~Registry()
        {
            _keep_alive->Cancel();
        }

        bool Registry_Service(const std::string &key, std::string &val)
        {
            auto resp = _client->put(key, val, _lease_id).get();
            if (resp.is_ok() == false)
            {
                LOG_ERROR("Registration data failed: {}", resp.error_message());
                return false;
            }

            return true;
        }

    private:
        std::shared_ptr<etcd::Client> _client;
        std::shared_ptr<etcd::KeepAlive> _keep_alive;
        uint64_t _lease_id;
    }; // end of Registry

    class Discovery
    {
    public:
        using ptr = std::shared_ptr<Discovery>;
        using NotifyCallback = std::function<void(std::string, std::string)>;

        Discovery(const std::string &host, const std::string &baseDir,
                  const NotifyCallback &put_cb, const NotifyCallback &del_cb)
            : _client(std::make_shared<etcd::Client>(host))
            , _put_cb(put_cb)
            , _del_cb(del_cb)
        {
            // Service discovery
            auto resp = _client->ls(baseDir).get();
            if (resp.is_ok() == false)
            {
                LOG_ERROR("Failed to get service information data: {}", resp.error_message());
            }

            int size = resp.keys().size();
            for (int i = 0; i < size; ++i)
            {
                if (_put_cb)
                {
                    _put_cb(resp.key(i), resp.value(i).as_string());
                }
            }

            // Event monitoring is carried out to monitor changes in data and call callbacks for processing
            _watcher = std::make_shared<etcd::Watcher>(*(_client.get()), baseDir,
                                                       std::bind(&Discovery::Callback, this, std::placeholders::_1), true);
        }

        ~Discovery()
        {
            _watcher->Cancel();
        }

    private:
        void Callback(const etcd::Response &resp)
        {
            if (resp.is_ok() == false)
            {
                LOG_ERROR("Received an error event notification: {}", resp.error_message());
                return;
            }

            for (const auto &ev : resp.events())
            {
                if (ev.event_type() == etcd::Event::EventType::PUT)
                {
                    if (_put_cb)
                    {
                        _put_cb(ev.kv().key(), ev.kv().as_string());
                    }

                    LOG_DEBUG("New Service: {}-{}", ev.kv().key(), ev.kv().as_string());
                }
                else if (ev.event_type() == etcd::Event::EventType::DELETE_)
                {
                    if (_del_cb)
                    {
                        _del_cb(ev.prev_kv().key(), ev.prev_kv().as_string());
                    }

                    LOG_DEBUG("Offline Service: {}-{}", ev.prev_kv().key(), ev.prev_kv().as_string());
                }
            }
        }

    private:
        NotifyCallback _put_cb;
        NotifyCallback _del_cb;
        std::shared_ptr<etcd::Client> _client;
        std::shared_ptr<etcd::Watcher> _watcher;
    }; // end of Discovery
} // end of namespace SnowK
