#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <brpc/channel.h>
#include "logger.hpp"

namespace SnowK
{
    // A channel management class for a single service
    // A service may be provided by multiple nodes, each node has its own channel
    class ServiceChannel
    {
    public:
        using ChannelPtr = std::shared_ptr<brpc::Channel>;
        using ptr = std::shared_ptr<ServiceChannel>;

        ServiceChannel(const std::string &name)
            : _service_name(name)
            , _index(0)
        {}

        ~ServiceChannel()
        {}

        // The service has launched a node and added a new channel
        void Append(const std::string &host)
        {
            brpc::ChannelOptions options;
            options.connect_timeout_ms = -1;
            options.timeout_ms = -1;
            options.max_retry = 3;
            options.protocol = "baidu_std";

            auto channel = std::make_shared<brpc::Channel>();
            if (channel->Init(host.c_str(), &options) == -1)
            {
                LOG_ERROR("Failed to initialize {} - {} channel", _service_name, host);
                return;
            }

            std::unique_lock<std::mutex> lock(_mutex);
            _hosts.insert(std::make_pair(host, channel));
            _channels.push_back(channel);
        }

        // The service takes a node offline and releases the channel
        void Remove(const std::string &host)
        {
            std::unique_lock<std::mutex> lock(_mutex);

            auto iter = _hosts.find(host);
            if(iter == _hosts.end())
            {
                LOG_WARN("When node {} - {} deletes a channel, no channel information is found",
                         _service_name, host);
                return;
            }

            for (auto viter = _channels.begin(); viter != _channels.end(); ++viter)
            {
                if(*viter == iter->second)
                {
                    _channels.erase(viter);
                    break;
                }
            }
            _hosts.erase(iter);
        }

        // Use the RR rotation policy to obtain a channel that 
            // can be used to initiate an RPC call to the corresponding service
        ChannelPtr Choose()
        {
            if(_channels.size() == 0)
            {
                LOG_ERROR("There are currently no nodes capable of providing {} services", _service_name);
                return ChannelPtr();
            }

            std::unique_lock<std::mutex> lock(_mutex);
            int32_t index = _index++ % _channels.size(); // RR
            
            return _channels[index];
        }

    private: 
        std::mutex _mutex;
        int32_t _index;                                     // Current rotation index
        std::string _service_name;
        std::vector<ChannelPtr> _channels;                  // Set of channels corresponding to the current service
        std::unordered_map<std::string, ChannelPtr> _hosts; // Host address to channel mapping, in order to delete
    }; // class ServiceChannel

    // All service channel management class
    class ServiceManager
    {
    public:
        using ptr = std::shared_ptr<ServiceManager>;

        ServiceManager()
        {}

        ~ServiceManager()
        {}

        // Declare the services that need to be managed
            // undeclared services don't need to be managed
        void Declare(const std::string &service_name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _follow_services.insert(service_name);
        }

        // Obtain the channel of the specified service node
        ServiceChannel::ChannelPtr Choose(const std::string &service_name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            
            auto siter = _services.find(service_name);
            if(siter == _services.end())
            {
                LOG_ERROR("There are currently no nodes capable of providing {} services", service_name);
                return ServiceChannel::ChannelPtr();
            }

            return siter->second->Choose();
        }

        // The callback API that is called when 
            // the service goes online, manages the service nodes
        void ServiceOnline(const std::string &service_instance, const std::string &host)
        {
            std::string service_name = GetServiceName(service_instance);
            auto fiter = _follow_services.find(service_name);
            if (fiter == _follow_services.end())
            {
                LOG_DEBUG("Service {} - {} is online, but it doesn't care at the moment", service_name, host);
                return;
            }

            ServiceChannel::ptr service;
            {
                std::unique_lock<std::mutex> lock(_mutex);

                // Get the management object first, create it if 
                    // it does not exist, and add the node if it exists.
                auto siter = _services.find(service_name);
                if(siter == _services.end())
                {
                    service = std::make_shared<ServiceChannel>(service_name);
                    _services.insert(std::make_pair(service_name, service));
                }
                else
                {
                    service = siter->second;
                }
            }

            if(!service)
            {
                LOG_ERROR("Failed to add {} service management node", service_name);
                return;
            }

            service->Append(host);
            LOG_DEBUG("{}-{} service launches a new node for addition management", service_name, host);
        }

        // The callback API called when a service goes offline 
            // deletes the specified node channel from the service channel management
        void ServiceOffline(const std::string &service_instance, const std::string &host)
        {
            std::string service_name = GetServiceName(service_instance);
            auto fiter = _follow_services.find(service_name);
            if (fiter == _follow_services.end())
            {
                LOG_DEBUG("Service {} - {} is offline, but it doesn't care at the moment", service_name, host);
                return;
            }

            ServiceChannel::ptr service;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                
                auto siter = _services.find(service_name);
                if (siter == _services.end())
                {
                    LOG_WARN("When deleting the {} service node, no management objects were found", service_name);
                    return;
                }

                service = siter->second;
            }

            service->Remove(host);
            LOG_DEBUG("{} - {} service offline node, delete this node", service_name, host);
        }

    private:
        std::string GetServiceName(const std::string &service_instance)
        {
            auto pos = service_instance.find_last_of('/');
            if(pos == std::string::npos)
            {
                return service_instance;
            }

            return service_instance.substr(0, pos);
        }

    private: 
        std::mutex _mutex;
        std::unordered_set<std::string> _follow_services;
        std::unordered_map<std::string, ServiceChannel::ptr> _services;
    };
} // namespace SnowK