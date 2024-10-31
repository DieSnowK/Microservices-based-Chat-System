#include "server.h"

//////////////////////////////////////////////////////////////////
/// HttpServer
//////////////////////////////////////////////////////////////////

HttpServer* HttpServer::instance = nullptr;

HttpServer *HttpServer::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new HttpServer();
    }

    return instance;
}

bool HttpServer::Init()
{
    // TODO
    int ret = httpServer.listen(QHostAddress::Any, 8000);

    // 配置路由
    httpServer.route("/ping", [](const QHttpServerRequest& req)
    {
        (void) req;
        qDebug() << "[http] A ping request is received";
        return "pong";
    });

    return ret == 8000;
}

//////////////////////////////////////////////////////////////////
/// WebsocketServer
//////////////////////////////////////////////////////////////////

WebsocketServer::WebsocketServer()
    : websocketServer("websocket server", QWebSocketServer::NonSecureMode)
{}
