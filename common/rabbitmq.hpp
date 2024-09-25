#pragma once
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include "logger.hpp"

namespace SnowK
{
    class MQClient
    {
    public:
        using MessageCallback = std::function<void(const char *, size_t)>;

        MQClient(const std::string &user, const std::string pwd, const std::string host)
        {
            _loop = EV_DEFAULT;
            _handler = std::make_unique<AMQP::LibEvHandler>(_loop);
            // std::string url = "amqp://" + user + ":" + passwd + "@" + host + "/";
            AMQP::Address address("amqp://" + user + ":" + pwd + "@" + host + "/");
            _connection = std::make_unique<AMQP::TcpConnection>(_handler.get(), address);
            _channel = std::make_unique<AMQP::TcpChannel>(_connection.get());

            _loop_thread = std::thread([this]()
                                       { ev_run(_loop, 0); });
        }

        ~MQClient()
        {
            ev_async_init(&_async_watcher, Watcher_Callback);
            ev_async_start(_loop, &_async_watcher);
            ev_async_send(_loop, &_async_watcher);
            _loop_thread.join();
        }

        void DeclareComponents(const std::string &exchange,
                               const std::string &queue,
                               const std::string &routing_key = "routing_key",
                               AMQP::ExchangeType echange_type = AMQP::ExchangeType::direct)
        {
            _channel->declareExchange(exchange, echange_type)
                .onError([](const char *message)
                {
                    LOG_ERROR("Failed to declare exchange: {}", message);
                    exit(0); 
                })
                .onSuccess([&exchange]()
                { 
                    LOG_INFO("Exchange {} is created", exchange); 
                });

            _channel->declareQueue(queue)
                .onError([](const char *message)
                {
                    LOG_ERROR("Failed to declare queue: {}", message);
                    exit(0); 
                })
                .onSuccess([&queue]()
                { 
                    LOG_INFO("Queue {} is created", queue); 
                });

            _channel->bindQueue(exchange, queue, routing_key)
                .onError([&exchange, &queue](const char *message)
                {
                    LOG_ERROR("Faild to bind {} - {}: {}", exchange, queue, message);
                    exit(0); 
                })
                .onSuccess([exchange, queue, routing_key]()
                { 
                    LOG_INFO("{} - {} - {} are binded", exchange, queue, routing_key); 
                });
        }

        bool Publish(const std::string &exchange,
                     const std::string &msg,
                     const std::string &routing_key = "routing_key")
        {
            if (_channel->publish(exchange, routing_key, msg) == false)
            {
                LOG_ERROR("{} failed to post message", exchange);
                return false;
            }
            LOG_INFO("Post a message to exchange {}-{}", exchange, routing_key);

            return true;
        }

        void Consume(const std::string &queue, const MessageCallback &cb)
        {
            LOG_INFO("Start subscribing to {} queue messages", queue);

            _channel->consume(queue, "consume-tag")
            .onReceived([this, cb](const AMQP::Message &message,
                                   uint64_t deliveryTag,
                                   bool redelivered)
            {
                    cb(message.body(), message.bodySize());
                    _channel->ack(deliveryTag); 
            })
            .onError([&queue](const char *message)
            {
                LOG_ERROR("Failed to subscribe to {} queue messages: {}", queue, message);
                exit(0); 
            });
        }

    private:
        // static is for the C interface
        static void Watcher_Callback(struct ev_loop *loop, ev_async *watcher, int32_t revents)
        {
            ev_break(loop, EVBREAK_ALL);
        }

    private:
        struct ev_async _async_watcher;
        struct ev_loop *_loop;
        std::unique_ptr<AMQP::LibEvHandler> _handler;
        std::unique_ptr<AMQP::TcpConnection> _connection;
        std::unique_ptr<AMQP::TcpChannel> _channel;
        std::thread _loop_thread;
    };
}