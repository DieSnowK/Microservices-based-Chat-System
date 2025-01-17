#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "logger.hpp"

namespace SnowK
{
    // The type of connection: server_t::connection_ptr
    typedef websocketpp::server<websocketpp::config::asio> server_t;

    class Connection
    {
    public:
        struct Client
        {
            Client(const std::string &u, const std::string &s) 
                : uid(u)
                , ssid(s) 
            {}
            
            std::string uid;
            std::string ssid;
        };

        using ptr = std::shared_ptr<Connection>;

        Connection() {}
        ~Connection() {}

        void Insert(const server_t::connection_ptr &conn,
                    const std::string &uid, const std::string &ssid)
        {
            std::unique_lock<std::mutex> lock(_mutex);

            _uid_connections.insert(std::make_pair(uid, conn));
            _conn_clients.insert(std::make_pair(conn, Client(uid, ssid)));

            LOG_DEBUG("New persistent connections user information:{}-{}-{}", 
                     (size_t)conn.get(), uid, ssid);
        }

        server_t::connection_ptr GetConnection(const std::string &uid)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            
            auto it = _uid_connections.find(uid);
            if (it == _uid_connections.end())
            {
                LOG_ERROR("No persistent connection found for {} client", uid);
                return server_t::connection_ptr();
            }
            
            LOG_DEBUG("Locate the persistent connection for the {} client", uid);
            return it->second;
        }

        // Obtained the persistent connection client information
        bool GetClientInfo(const server_t::connection_ptr &conn, std::string &uid, std::string &ssid)
        {
            std::unique_lock<std::mutex> lock(_mutex);

            auto it = _conn_clients.find(conn);
            if (it == _conn_clients.end())
            {
                LOG_ERROR("Client information for persistent connection {}\
                          not found", (size_t)conn.get());
                return false;
            }

            uid = it->second.uid;
            ssid = it->second.ssid;

            LOG_DEBUG("Obtained the persistent connection client information");
            return true;
        }

        void Remove(const server_t::connection_ptr &conn)
        {
            std::unique_lock<std::mutex> lock(_mutex);

            auto it = _conn_clients.find(conn);
            if (it == _conn_clients.end())
            {
                LOG_ERROR("Client information for persistent connection {} \
                          was not found", (size_t)conn.get());
                return;
            }

            _uid_connections.erase(it->second.uid);
            _conn_clients.erase(it);

            LOG_DEBUG("Deleted persistent connection information is complete");
        }

    private:
        std::mutex _mutex;
        std::unordered_map<std::string, server_t::connection_ptr> _uid_connections;
        std::unordered_map<server_t::connection_ptr, Client> _conn_clients;
    }; // end of class Connection
}