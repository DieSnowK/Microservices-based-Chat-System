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
    // Configure routes
    httpServer.route("/ping", [](const QHttpServerRequest& req)
    {
        (void) req;
        qDebug() << "[http] A ping request is received";
        return "Pong";
    });

    return tcpServer.listen(QHostAddress::Any, 8000) && httpServer.bind(&tcpServer);
}

//////////////////////////////////////////////////////////////////
/// WebsocketServer
//////////////////////////////////////////////////////////////////

WebsocketServer* WebsocketServer::instance = nullptr;

WebsocketServer *WebsocketServer::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new WebsocketServer();
    }
    return instance;
}

WebsocketServer::WebsocketServer()
    : websocketServer("websocket server", QWebSocketServer::NonSecureMode)
{}

bool WebsocketServer::Init()
{
    connect(&websocketServer, &QWebSocketServer::newConnection, this, [=]()
    {
        qDebug() << "[websocket] The connection was successfully established!";

        // Get the socket object used for communication
            // nextPendingConnection is similar to accept in a native socket
        QWebSocket* socket = websocketServer.nextPendingConnection();

        connect(socket, &QWebSocket::disconnected, this, [=]()
        {
            qDebug() << "[websocket] The connection is lost";
        });

        connect(socket, &QWebSocket::errorOccurred, this, [=](QAbstractSocket::SocketError error)
        {
            qDebug() << "[websocket] There was a connection error: " << error;
        });

        connect(socket, &QWebSocket::textMessageReceived, this, [=](const QString& message)
        {
            qDebug() << "[websocket] Text data is received, message = " << message;
        });

        connect(socket, &QWebSocket::binaryMessageReceived, this, [=](const QByteArray& byteArray)
        {
            qDebug() << "[websocket] Binary data is received! " << byteArray.length();
        });
    });


    return websocketServer.listen(QHostAddress::Any, 8001);
}
