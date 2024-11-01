#include "mainwindow.h"
#include "server.h"

#include <QApplication>

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



    MainWindow w;
    w.show();
    return a.exec();
}
