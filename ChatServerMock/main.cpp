#include <QApplication>

#include "widget.h"
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    HttpServer* httpServer = HttpServer::GetInstance();
    if(!httpServer->Init())
    {
        qDebug() << "The HTTP server failed to start";
        return 1;
    }
    qDebug() << "The HTTP server started successfully";

    WebsocketServer* websocketServer = WebsocketServer::GetInstance();
    if (!websocketServer->Init())
    {
        qDebug() << "The WebSocket server failed to start";
        return 1;
    }
    qDebug() << "The WebSocket server started successfully";

    Widget w;
    w.show();
    return a.exec();
}
