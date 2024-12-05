#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QWebSocket>
#include <QProtobufSerializer>
#include <QNetworkReply>

#include "../model/data.hpp"

using model::MessageType;

// To avoid the "loop contains" problem -> Pre-declaration
namespace model
{
    class DataCenter;
}

namespace network
{
    // Inside this class, the specific network communication is done
    class NetClient : public QObject
    {
        Q_OBJECT

    private:
        const QString HTTP_URL = "http://127.0.0.1:9000";
        const QString WEBSOCKET_URL = "ws://127.0.0.1:9001/ws";

    public:
        NetClient(model::DataCenter* dataCenter);
        void Ping();

        /////////////////////////////////////////////////////////////////////////////////
        /// Websocket
        /////////////////////////////////////////////////////////////////////////////////

        void InitWebsocket();

        void HandleWsResponse(const SnowK::NotifyMessage& notifyMessage);
        void HandleWsMessage(const model::Message& message);
        void HandleWsRemoveFriend(const QString& userId);
        void HandleWsAddFriendApply(const model::UserInfo& userInfo);
        void HandleWsAddFriendProcess(const model::UserInfo& userInfo, bool agree);
        void HandleWsSessionCreate(const model::ChatSessionInfo& chatSessionInfo);


        /////////////////////////////////////////////////////////////////////////////////
        /// HTTP
        /////////////////////////////////////////////////////////////////////////////////

        static QString MakeRequestId();
        QNetworkReply* SendHttpRequest(const QString& apiPath, const QByteArray& body);

        // Since different APIs have different structures for the returned PB objects
            // templates are needed in order for a function to handle many different types
        // The output parameter indicates whether the operation
            // succeeded or failed, and the reason for the failure
        template <typename T>
        std::shared_ptr<T> HandleHttpResponse(QNetworkReply* httpResp, bool* ok, QString* reason)
        {
            if (httpResp->error() != QNetworkReply::NoError)
            {
                *ok = false;
                *reason = httpResp->errorString();
                httpResp->deleteLater();
                return std::shared_ptr<T>();
            }

            QByteArray respBody = httpResp->readAll();

            std::shared_ptr<T> respObj = std::make_shared<T>();
            respObj->deserialize(&serializer, respBody);

            if (!respObj->success())
            {
                *ok = false;
                *reason = respObj->errmsg();
                httpResp->deleteLater();
                return std::shared_ptr<T>();
            }

            httpResp->deleteLater();
            *ok = true;
            return respObj;
        }

        void SendAuth();
        void GetMyself(const QString& loginSessionId);
        void GetFriendList(const QString& loginSessionId);
        void GetChatSessionList(const QString& loginSessionId);
        void GetApplyList(const QString& loginSessionId);
        void GetRecentMessageList(const QString& loginSessionId,
                                  const QString& chatSessionId, bool updateUI);
        void SendMessage(const QString& loginSessionId, const QString& chatSessionId,
                         MessageType messageType,const QByteArray& content, const QString& extraInfo);
        void ReceiveMessage(const QString& chatSessionId);
        void ChangeNickname(const QString& loginSessionId, const QString& nickname);
        void ChangeDescription(const QString& loginSessionId, const QString& desc);
        void GetVerifyCode(const QString& phone);
        void ChangePhone(const QString& loginSessionId, const QString& phone,
                         const QString& verifyCodeId, const QString& verifyCode);
        void ChangeAvatar(const QString& loginSessionId, const QByteArray& avatar);
        void DeleteFriend(const QString& loginSessionId, const QString& userId);
        void AddFriendApply(const QString& loginSessionId, const QString& userId);
        void AcceptFriendApply(const QString& loginSessionId, const QString& userId);
        void RejectFriendApply(const QString& loginSessionId, const QString& userId);
        void CreateGroupChatSession(const QString& loginSessionId, const QList<QString>& userIdList);
        void GetMemberList(const QString& loginSessionId, const QString& chatSessionId);

    private:
        model::DataCenter* dataCenter;
        QNetworkAccessManager httpClient;
        QWebSocket websocketClient;
        QProtobufSerializer serializer;
    }; // end of class NetClient
} // end of namespace network

#endif // NETCLIENT_H
