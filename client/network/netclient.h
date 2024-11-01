#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QWebSocket>
#include <QProtobufSerializer>
#include <QNetworkReply>

// To avoid the "loop contains" problem -> Pre-declaration
namespace model
{
    class DataCenter;
}

namespace network
{
    class NetClient : public QObject
    {
        Q_OBJECT

    private:
        const QString HTTP_URL = "http://127.0.0.1:8000";
        const QString WEBSOCKET_URL = "ws://127.0.0.1:8001/ws";

    public:
        NetClient(model::DataCenter* dataCenter);

        void InitWebsocket();

        static QString MakeRequestId();

        void Ping();
        void SendAuth();

    private:
        model::DataCenter* dataCenter;
        QNetworkAccessManager httpClient;
        QWebSocket websocketClient;
        QProtobufSerializer serializer;
    }; // end of class NetClient
} // end of namespace network

#endif // NETCLIENT_H
