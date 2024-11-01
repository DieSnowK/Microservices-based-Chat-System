#include "netclient.h"
#include "../model/data.hpp"
#include "../model/datacenter.h"

namespace network
{
    NetClient::NetClient(model::DataCenter *dataCenter)
        : dataCenter(dataCenter)
    {
        InitWebsocket();
    }

    void NetClient::InitWebsocket()
    {
        connect(&websocketClient, &QWebSocket::connected, this, [=]()
        {
            LOG() << "The websocket connection is successful";
            SendAuth();
        });

        connect(&websocketClient, &QWebSocket::disconnected, this, [=]()
        {
            LOG() << "The websocket connection is dropped";
        });

        connect(&websocketClient, &QWebSocket::errorOccurred, this, [=](QAbstractSocket::SocketError error)
        {
            LOG() << "There was an error with the websocket connection: " << error;
        });

        connect(&websocketClient, &QWebSocket::textMessageReceived, this, [=](const QString& message)
        {
            LOG() << "The websocket receives a text message: " << message;
        });

        connect(&websocketClient, &QWebSocket::binaryMessageReceived, this, [=](const QByteArray& byteArray)
        {
            LOG() << "The websocket receives a binary message: " << byteArray.length();
            // SnowK::NotifyMessage notifyMessage;
            // notifyMessage.deserialize(&serializer, byteArray);
            // handleWsResponse(notifyMessage);
        });

        websocketClient.open(WEBSOCKET_URL);
    }

    QString NetClient::MakeRequestId()
    {
        return "R" + QUuid::createUuid().toString().sliced(25, 12);
    }

    void NetClient::Ping()
    {
        QNetworkRequest httpReq;
        httpReq.setUrl(QUrl(HTTP_URL + "/ping"));

        QNetworkReply* httpResp = httpClient.get(httpReq);

        // When the slot function is triggered, the response has returned
        connect(httpResp, &QNetworkReply::finished, this, [=]()
        {
            if (httpResp->error() != QNetworkReply::NoError)
            {
                LOG() << "The HTTP request failed: " << httpResp->errorString();
                httpResp->deleteLater();
                return;
            }

            QByteArray body = httpResp->readAll();
            LOG() << "The content of the response: " << body;

            httpResp->deleteLater();
        });
    }

    void NetClient::SendAuth()
    {
        SnowK::ClientAuthenticationReq req;
        req.setRequestId(MakeRequestId());
        req.setSessionId(dataCenter->GetLoginSessionId());
        QByteArray body = req.serialize(&serializer);

        LOG() << "[WS Authentication] requestId = " << req.requestId()
              << ", loginSessionId = " << req.sessionId();

        websocketClient.sendBinaryMessage(body);
    }
} // end of namespace network
