#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QHttpServer>
#include <QProtobufSerializer>
#include <QWebSocketServer>
#include <QFileInfo>
#include <QFile>
#include <QPixmap>
#include <QIcon>

//////////////////////////////////////////////////////////////////
/// HttpServer
//////////////////////////////////////////////////////////////////

class HttpServer : public QObject
{
    Q_OBJECT

public:
    static HttpServer* GetInstance();
    bool Init();

private:
    HttpServer() {}
    static HttpServer* instance;

    QTcpServer tcpServer;
    QHttpServer httpServer;
    QProtobufSerializer serializer;
};

//////////////////////////////////////////////////////////////////
/// WebsocketServer
//////////////////////////////////////////////////////////////////

class WebsocketServer : public QObject
{
    Q_OBJECT

public:
    static WebsocketServer* GetInstance();
    bool Init();

private:
    static WebsocketServer* instance;
    WebsocketServer();

    QWebSocketServer websocketServer;
    QProtobufSerializer serializer;

    int messageIndex = 0;

signals:
    // void SendTextResp();
    // void SendImageResp();
    // void SendFileResp();
    // void SendSpeechResp();
    // void SendFriendRemove();
    // void SendAddFriendApply();
    // void SendAddFriendProcess(bool agree);
    // void SendCreateChatSession();
};

#endif // SERVER_H
