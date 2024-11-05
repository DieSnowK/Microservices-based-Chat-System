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

    QNetworkReply *NetClient::SendHttpRequest(const QString &apiPath, const QByteArray &body)
    {
        QNetworkRequest httpReq;
        httpReq.setUrl(QUrl(HTTP_URL + apiPath));
        httpReq.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-protobuf");

        QNetworkReply* httpResp = httpClient.post(httpReq, body);
        return httpResp;
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

    void NetClient::GetMyself(const QString &loginSessionId)
    {
        SnowK::GetUserInfoReq req;
        req.setRequestId(MakeRequestId());
        req.setSessionId(loginSessionId);
        QByteArray body = req.serialize(&serializer);
        LOG() << "[GetMyself] Send a request requestId = "
              << req.requestId() << ", loginSessionId = " << loginSessionId;

        QNetworkReply* httpResp = SendHttpRequest("/service/user/get_user_info", body);

        connect(httpResp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto resp = HandleHttpResponse<SnowK::GetUserInfoRsp>(httpResp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[GetMyself] Error, requestId = " << req.requestId() << "reason = " << reason;
                return;
            }

            dataCenter->ResetMyself(resp);
            emit dataCenter->GetMyselfDone();

            LOG() << "[GetMyself] Process the response done, requestId = " << req.requestId();
        });
    }

    void NetClient::GetFriendList(const QString &loginSessionId)
    {
        SnowK::GetFriendListReq req;
        req.setRequestId(MakeRequestId());
        req.setSessionId(loginSessionId);
        QByteArray body = req.serialize(&serializer);
        LOG() << "[GetFriendList] Send a request requestId requestId = "
              << req.requestId() << ", loginSessionId=" << loginSessionId;

        QNetworkReply* httpResp = this->SendHttpRequest("/service/friend/get_friend_list", body);

        connect(httpResp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto friendListResp = this->HandleHttpResponse<SnowK::GetFriendListRsp>(httpResp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[GetFriendList] Error, requestId = " << req.requestId() << ", reason = " << reason;
                return;
            }

            dataCenter->ResetFriendList(friendListResp);
            emit dataCenter->GetFriendListDone();

            LOG() << "[GetFriendList] Process the response done, requestId = " << req.requestId();
        });
    }

    void NetClient::GetChatSessionList(const QString &loginSessionId)
    {
        SnowK::GetChatSessionListReq req;
        req.setRequestId(MakeRequestId());
        req.setSessionId(loginSessionId);
        QByteArray body = req.serialize(&serializer);
        LOG() << "[GetChatSessionList] Send a request requestId requestId = "
              << req.requestId() << ", loginSessionId = " << loginSessionId;

        QNetworkReply* resp = this->SendHttpRequest("/service/friend/get_chat_session_list", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::GetChatSessionListRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[GetChatSessionList] Error, reason=" << reason;
                return;
            }

            dataCenter->ResetChatSessionList(pbResp);
            emit dataCenter->GetChatSessionListDone();

            LOG() << "[GetChatSessionList] Process the response done, requestId=" << pbResp->requestId();
        });
    }

    // TODO
    void NetClient::GetApplyList(const QString &loginSessionId)
    {
        SnowK::GetPendingFriendEventListReq req;
        req.setRequestId(MakeRequestId());
        req.setSessionId(loginSessionId);
        QByteArray body = req.serialize(&serializer);
        LOG() << "[GetApplyList] Send a request requestId requestId = "
              << req.requestId() << ", loginSessionId = " << loginSessionId;

        QNetworkReply* resp = this->SendHttpRequest("/service/friend/get_pending_friend_events", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::GetPendingFriendEventListRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[GetApplyList] Error, reason: " << reason;
                return;
            }

            dataCenter->ResetApplyList(pbResp);
            emit dataCenter->GetApplyListDone();

            LOG() << "[GetApplyList] Process the response done, requestId=" << req.requestId();
        });
    }

    void NetClient::GetRecentMessageList(const QString &loginSessionId,
                                         const QString &chatSessionId, bool updateUI)
    {
        SnowK::GetRecentMsgReq req;
        req.setRequestId(MakeRequestId());
        req.setChatSessionId(chatSessionId);
        req.setMsgCount(50);    // Get the last 50 records here in a pin
        req.setSessionId(loginSessionId);
        QByteArray body = req.serialize(&serializer);
        LOG() << "[GetRecentMessageList] Send a request requestId requestId = "
              << req.requestId() << ", loginSessionId = " << loginSessionId
              << ", chatSessionId = " << chatSessionId;

        QNetworkReply* resp = this->SendHttpRequest("/service/message_storage/get_recent", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::GetRecentMsgRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[GetRecentMessageList] Error, reason = " << reason;
                return;
            }

            dataCenter->ResetRecentMessageList(chatSessionId, pbResp);
            if (updateUI)
            {
                emit dataCenter->GetRecentMessageListDone(chatSessionId);
            }
            else
            {
                emit dataCenter->GetRecentMessageListDoneNoUI(chatSessionId);
            }
        });
    }
} // end of namespace network
