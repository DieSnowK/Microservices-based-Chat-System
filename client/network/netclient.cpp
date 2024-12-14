#include "netclient.h"
#include "../model/data.hpp"
#include "../model/datacenter.h"

using namespace model;

namespace network
{
    NetClient::NetClient(model::DataCenter *dataCenter)
        : dataCenter(dataCenter)
    {
        // Error, It shouldn't be initialized here, it should be in the MainWidget
        // InitWebsocket();
    }

    /////////////////////////////////////////////////////////////////////////////////
    /// Websocket
    /////////////////////////////////////////////////////////////////////////////////

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

            SnowK::NotifyMessage notifyMessage;
            notifyMessage.deserialize(&serializer, byteArray);
            HandleWsResponse(notifyMessage);
        });

        websocketClient.open(WEBSOCKET_URL);
    }

    void NetClient::HandleWsResponse(const SnowK::NotifyMessage &notifyMessage)
    {
        if (notifyMessage.notifyType() == SnowK::NotifyTypeGadget::NotifyType::CHAT_MESSAGE_NOTIFY)
        {
            Message message;
            message.Load(notifyMessage.newMessageInfo().messageInfo());
            HandleWsMessage(message);
        }
        else if (notifyMessage.notifyType() == SnowK::NotifyTypeGadget::NotifyType::CHAT_SESSION_CREATE_NOTIFY)
        {
            ChatSessionInfo chatSessionInfo;
            chatSessionInfo.Load(notifyMessage.newChatSessionInfo().chatSessionInfo());
            HandleWsSessionCreate(chatSessionInfo);
        }
        else if (notifyMessage.notifyType() == SnowK::NotifyTypeGadget::NotifyType::FRIEND_ADD_APPLY_NOTIFY)
        {
            UserInfo userInfo;
            userInfo.Load(notifyMessage.friendAddApply().userInfo());
            HandleWsAddFriendApply(userInfo);
        }
        else if (notifyMessage.notifyType() == SnowK::NotifyTypeGadget::NotifyType::FRIEND_ADD_PROCESS_NOTIFY)
        {
            UserInfo userInfo;
            userInfo.Load(notifyMessage.friendProcessResult().userInfo());
            bool agree = notifyMessage.friendProcessResult().agree();
            HandleWsAddFriendProcess(userInfo, agree);
        }
        else if (notifyMessage.notifyType() == SnowK::NotifyTypeGadget::NotifyType::FRIEND_REMOVE_NOTIFY)
        {
            const QString& userId = notifyMessage.friendRemove().userId();
            HandleWsRemoveFriend(userId);
        }
    }

    void NetClient::HandleWsMessage(const model::Message &message)
    {
        QList<Message>* messageList = dataCenter->GetRecentMessageList(message.chatSessionId);
        if (messageList == nullptr)
        {
            // If the message list in the current session is not loaded locally,
                // need to load the entire message list over the network
            connect(dataCenter, &DataCenter::GetRecentMessageListDoneNoUI,
                    this, &NetClient::ReceiveMessage, Qt::UniqueConnection);
            dataCenter->GetRecentMessageListAsync(message.chatSessionId, false);
        }
        else
        {
            // If the message list in the current conversation has been loaded
                // locally, can insert the message tail into the message list
            messageList->push_back(message);
            this->ReceiveMessage(message.chatSessionId);
        }
    }

    void NetClient::HandleWsRemoveFriend(const QString &userId)
    {
        dataCenter->RemoveFriend(userId);
        emit dataCenter->DeleteFriendDone();
    }

    void NetClient::HandleWsAddFriendApply(const model::UserInfo &userInfo)
    {
        QList<UserInfo>* applyList = dataCenter->GetApplyList();
        if (applyList == nullptr)
        {
            LOG() << "The client does not load the friend application list!";
            return;
        }

        applyList->push_front(userInfo);
        emit dataCenter->ReceiveFriendApplyDone();
    }

    void NetClient::HandleWsAddFriendProcess(const model::UserInfo &userInfo, bool agree)
    {
        if (agree)
        {
            QList<UserInfo>* friendList = dataCenter->GetFriendList();
            if (friendList == nullptr)
            {
                LOG() << "The client does not load the friend application list!";
                return;
            }
            friendList->push_front(userInfo);
        }

        emit dataCenter->ReceiveFriendProcessDone(userInfo.nickname, agree);
    }

    void NetClient::HandleWsSessionCreate(const model::ChatSessionInfo &chatSessionInfo)
    {
        QList<ChatSessionInfo>* chatSessionList = dataCenter->GetChatSessionList();
        if (chatSessionList == nullptr)
        {
            LOG() << "The client did not load the session list";
            return;
        }

        chatSessionList->push_front(chatSessionInfo);
        emit dataCenter->ReceiveSessionCreateDone();
    }

    /////////////////////////////////////////////////////////////////////////////////
    /// HTTP
    /////////////////////////////////////////////////////////////////////////////////

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

        LOG() << "[GetMyself] Send a request, requestId = "
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

        LOG() << "[GetFriendList] Send a request, requestId = "
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

        LOG() << "[GetChatSessionList] Send a request, requestId = "
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

        LOG() << "[GetApplyList] Send a request, requestId = "
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

        LOG() << "[GetRecentMessageList] Send a request, requestId = "
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

        LOG() << "[GetRecentMessageList] Process the response done, requestId=" << req.requestId();
    }

    void NetClient::SendMessage(const QString &loginSessionId, const QString &chatSessionId,
                                MessageType messageType, const QByteArray &content, const QString &extraInfo)
    {
        SnowK::NewMessageReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setChatSessionId(chatSessionId);

        SnowK::MessageContent messageContent;
        if (messageType == MessageType::TEXT_TYPE)
        {
            messageContent.setMessageType(SnowK::MessageTypeGadget::MessageType::STRING);

            SnowK::StringMessageInfo stringMessageInfo;
            stringMessageInfo.setContent(content);
            messageContent.setStringMessage(stringMessageInfo);
        }
        else if (messageType == MessageType::IMAGE_TYPE)
        {
            messageContent.setMessageType(SnowK::MessageTypeGadget::MessageType::IMAGE);

            SnowK::ImageMessageInfo imageMessageInfo;
            imageMessageInfo.setFileId("");
            imageMessageInfo.setImageContent(content);
            messageContent.setImageMessage(imageMessageInfo);
        }
        else if (messageType == MessageType::FILE_TYPE)
        {
            messageContent.setMessageType(SnowK::MessageTypeGadget::MessageType::FILE);

            SnowK::FileMessageInfo fileMessageInfo;
            fileMessageInfo.setFileId("");
            fileMessageInfo.setFileSize(content.size());
            fileMessageInfo.setFileName(extraInfo);
            fileMessageInfo.setFileContents(content);
            messageContent.setFileMessage(fileMessageInfo);
        }
        else if (messageType == MessageType::SPEECH_TYPE)
        {
            messageContent.setMessageType(SnowK::MessageTypeGadget::MessageType::SPEECH);

            SnowK::SpeechMessageInfo speechMessageInfo;
            speechMessageInfo.setFileId("");
            speechMessageInfo.setFileContents(content);
            messageContent.setSpeechMessage(speechMessageInfo);
        }
        else
        {
            LOG() << "Error MessageType, messageType = " << (int)messageType;
        }
        pbReq.setMessage(messageContent);

        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[SendMessage] Send a request requestId requestId = "
              << pbReq.requestId() << ", loginSessionId=" << pbReq.sessionId()
              << ", chatSessionId = " << pbReq.chatSessionId()
              << ", messageType = " << pbReq.message().messageType();

        QNetworkReply* resp = this->SendHttpRequest("/service/message_transmit/new_message", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::NewMessageRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[SendMessage] Error, reason = " << reason;
                return;
            }

            emit dataCenter->SendMessageDone(messageType, content, extraInfo);

            LOG() << "[GetRecentMessageList] Process the response done, requestId=" << pbReq.requestId();
        });
    }

    void NetClient::ReceiveMessage(const QString &chatSessionId)
    {
        if (chatSessionId == dataCenter->GetCurrentChatSessionId())
        {
            const Message& lastMessage = dataCenter->GetRecentMessageList(chatSessionId)->back();
            emit dataCenter->ReceiveMessageDone(lastMessage);
        }
        else
        {
            dataCenter->AddUnread(chatSessionId);
        }

        emit dataCenter->UpdateLastMessage(chatSessionId);
    }

    void NetClient::ChangeNickname(const QString &loginSessionId, const QString &nickname)
    {
        SnowK::SetUserNicknameReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setNickname(nickname);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[ChangeNickname] Send a request, requestId = " << pbReq.requestId()
              << ", loginSessionId=" << pbReq.sessionId() << ", nickname=" << pbReq.nickname();

        QNetworkReply* resp = this->SendHttpRequest("/service/user/set_nickname", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::SetUserNicknameRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[ChangeNickname] Error, reason=" << reason;
                return;
            }

            dataCenter->ResetNickname(nickname);
            emit dataCenter->ChangeNicknameDone();

            LOG() << "[ChangeNickname] Process the response done, requestId=" << pbResp->requestId();
        });
    }

    void NetClient::ChangeDescription(const QString &loginSessionId, const QString &desc)
    {
        SnowK::SetUserDescriptionReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setDescription(desc);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[ChangeDescription] Send a request, requestId = " << pbReq.requestId()
              << ", loginSessisonId=" << pbReq.sessionId() << ", desc=" << pbReq.description();

        QNetworkReply* resp = this->SendHttpRequest("/service/user/set_description", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::SetUserDescriptionRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[ChangeDescription] Error, reason=" << reason;
                return;
            }

            dataCenter->ResetDescription(desc);
            emit dataCenter->ChangeDescriptionDone();

            LOG() << "[ChangeDescription] Process the response done, requestId=" << pbResp->requestId();
        });
    }

    void NetClient::GetVerifyCode(const QString &phone)
    {
        SnowK::PhoneVerifyCodeReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setPhoneNumber(phone);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[GetVerifyCode] Send a request, requestId = " << pbReq.requestId() << ", phone=" << phone;

        QNetworkReply* resp = this->SendHttpRequest("/service/user/get_phone_verify_code", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::PhoneVerifyCodeRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[GetVerifyCode] Error, reason=" << reason;
                return;
            }

            dataCenter->ResetVerifyCodeId(pbResp->verifyCodeId());
            emit dataCenter->GetVerifyCodeDone();

            LOG() << "[GetVerifyCode] Process the response done, requestId=" << pbResp->requestId();
        });
    }

    void NetClient::ChangePhone(const QString &loginSessionId, const QString &phone,
                                const QString &verifyCodeId, const QString &verifyCode)
    {
        SnowK::SetUserPhoneNumberReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setPhoneNumber(phone);
        pbReq.setPhoneVerifyCodeId(verifyCodeId);
        pbReq.setPhoneVerifyCode(verifyCode);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[ChangePhone] Send a request, requestId = " << pbReq.requestId()
              << ", loginSessionId=" << pbReq.sessionId() << ", phone=" << pbReq.phoneNumber()
              << ", verifyCodeId=" << pbReq.phoneVerifyCodeId() << ", verifyCode=" << pbReq.phoneVerifyCode();

        QNetworkReply* resp = this->SendHttpRequest("/service/user/set_phone", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::SetUserPhoneNumberRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[ChangePhone] Error, reason=" << reason;
                return;
            }

            dataCenter->ResetPhone(phone);
            emit dataCenter->ChangePhoneDone();

            LOG() << "[ChangePhone] Process the response done, requestId" << pbResp->requestId();
        });
    }

    void NetClient::ChangeAvatar(const QString &loginSessionId, const QByteArray &avatar)
    {
        SnowK::SetUserAvatarReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setAvatar(avatar);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[ChangeAvatar] Send a request, requestId = " << pbReq.requestId()
              << ", loginSessionId=" << pbReq.sessionId();

        QNetworkReply* resp = this->SendHttpRequest("/service/user/set_avatar", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::SetUserAvatarRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[ChangeAvatar] Error, reason=" << reason;
                return;
            }

            dataCenter->ResetAvatar(avatar);
            emit dataCenter->ChangeAvatarDone();

            LOG() << "[ChangeAvatar] Process the response done, requestId" << pbResp->requestId();
        });
    }

    void NetClient::DeleteFriend(const QString &loginSessionId, const QString &userId)
    {
        SnowK::FriendRemoveReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setPeerId(userId);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[DeleteFriend] Send a request, requestId = " << pbReq.requestId()
              << ", loginSessionId=" << pbReq.sessionId() << ", peerId=" << pbReq.peerId();

        QNetworkReply* resp = this->SendHttpRequest("/service/friend/remove_friend", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::FriendRemoveRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[DeleteFriend] Error, reason=" << reason;
                return;
            }

            dataCenter->RemoveFriend(userId);
            emit dataCenter->DeleteFriendDone();

            LOG() << "[DeleteFriend] Process the response done, requestId" << pbResp->requestId();
        });
    }

    void NetClient::AddFriendApply(const QString &loginSessionId, const QString &userId)
    {
        SnowK::FriendAddReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setRespondentId(userId);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[AddFriendApply] Send a request, requestId = " << pbReq.requestId()
              << ", loginSessionId=" << pbReq.sessionId() << ", userId=" << userId;

        QNetworkReply* resp = this->SendHttpRequest("/service/friend/add_friend_apply", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::FriendAddRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[AddFriendApply] Error, reason=" << reason;
                return;
            }

            emit dataCenter->AddFriendApplyDone();

            LOG() << "[AddFriendApply] Process the response done, requestId" << pbResp->requestId();
        });
    }

    void NetClient::AcceptFriendApply(const QString &loginSessionId, const QString &userId)
    {
        SnowK::FriendAddProcessReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setAgree(true);
        pbReq.setApplyUserId(userId);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[AcceptFriendApply] Send a request, requestId = " << pbReq.requestId() << ", loginSessionId="
              << pbReq.sessionId() << ", userId=" << pbReq.applyUserId();

        QNetworkReply* resp = this->SendHttpRequest("/service/friend/add_friend_process", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::FriendAddRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[AcceptFriendApply] Error, reason=" << reason;
                return;
            }

            UserInfo applyUser = dataCenter->RemoveFromApplyList(userId);
            QList<UserInfo>* friendList = dataCenter->GetFriendList();
            friendList->push_front(applyUser);

            emit dataCenter->AcceptFriendApplyDone();

            LOG() << "[AcceptFriendApply] Process the response done, requestId" << pbResp->requestId();
        });
    }

    void NetClient::RejectFriendApply(const QString &loginSessionId, const QString &userId)
    {
        SnowK::FriendAddProcessReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setAgree(false);
        pbReq.setApplyUserId(userId);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[RejectFriendApply] Send a request, requestId = " << pbReq.requestId() << ", loginSessionId="
              << pbReq.sessionId() << ", userId=" << pbReq.applyUserId();

        QNetworkReply* resp = this->SendHttpRequest("/service/friend/add_friend_process", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::FriendAddRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[RejectFriendApply] Error, reason=" << reason;
                return;
            }

            dataCenter->RemoveFromApplyList(userId);
            emit dataCenter->RejectFriendApplyDone();

            LOG() << "[RejectFriendApply] Process the response done, requestId" << pbResp->requestId();
        });
    }

    void NetClient::CreateGroupChatSession(const QString &loginSessionId, const QList<QString> &userIdList)
    {
        SnowK::ChatSessionCreateReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setChatSessionName("New Group Chat");
        pbReq.setMemberIdList(userIdList);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[CreateGroupChatSession] Send a request, requestId = " << pbReq.requestId()
              << ", loginSessionId=" << loginSessionId << ", userIdList=" << userIdList;

        QNetworkReply* resp = this->SendHttpRequest("/service/friend/create_chat_session", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::ChatSessionCreateRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[CreateGroupChatSession] Error, reason=" << reason;
                return;
            }

            // WebSocket...
            emit dataCenter->CreateGroupChatSessionDone();

            LOG() << "[CreateGroupChatSession] Process the response done, requestId = " << pbResp->requestId();
        });
    }

    void NetClient::GetMemberList(const QString &loginSessionId, const QString &chatSessionId)
    {
        SnowK::GetChatSessionMemberReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setChatSessionId(chatSessionId);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[GetMemberList] Send a request, requestId = " << pbReq.requestId() << ", loginSessionId="
              << pbReq.sessionId() << ", chatSessionId=" << pbReq.chatSessionId();

        QNetworkReply* resp = this->SendHttpRequest("/service/friend/get_chat_session_member", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::GetChatSessionMemberRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[GetMemberList] Error, reason=" << reason;
                return;
            }

            dataCenter->ResetMemberList(chatSessionId, pbResp->memberInfoList());
            emit dataCenter->GetMemberListDone(chatSessionId);

            LOG() << "[GetMemberList] Process the response done, requestId = " << pbResp->requestId();
        });
    }

    void NetClient::SearchUser(const QString &loginSessionId, const QString &searchKey)
    {
        SnowK::FriendSearchReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setSearchKey(searchKey);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[SearchUser] Send a request, requestId = " << pbReq.requestId() << ", loginSessionId="
              << loginSessionId << ", searchKey=" << searchKey;

        QNetworkReply* resp = this->SendHttpRequest("/service/friend/search_friend", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::FriendSearchRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[SearchUser] Error, reason=" << reason;
                return;
            }

            dataCenter->ResetSearchUserResult(pbResp->userInfo());
            emit dataCenter->SearchUserDone();

            LOG() << "[SearchUser] Process the response done, requestId = " << pbResp->requestId();
        });
    }

    void NetClient::SearchMessage(const QString &loginSessionId, const QString &chatSessionId, const QString &searchKey)
    {
        SnowK::MsgSearchReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setChatSessionId(chatSessionId);
        pbReq.setSearchKey(searchKey);
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[SearchMessage] Send a request, requestId = " << pbReq.requestId() << ", loginSessionId="
              << pbReq.sessionId() << ", chatSessionId=" << pbReq.chatSessionId() << ", searchKey=" << searchKey;

        QNetworkReply* resp = this->SendHttpRequest("/service/message_storage/search_history", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::MsgSearchRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[SearchMessage] Error, reason=" << reason;
                return;
            }


            dataCenter->ResetSearchMessageResult(pbResp->msgList());
            emit dataCenter->SearchMessageDone();

            LOG() << "[SearchMessage] Process the response done, requestId = " << pbResp->requestId();
        });
    }

    void NetClient::SearchMessageByTime(const QString &loginSessionId, const QString &chatSessionId,
                                        const QDateTime &begTime, const QDateTime &endTime)
    {
        SnowK::GetHistoryMsgReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setSessionId(loginSessionId);
        pbReq.setChatSessionId(chatSessionId);
        pbReq.setStartTime(begTime.toSecsSinceEpoch());
        pbReq.setOverTime(endTime.toSecsSinceEpoch());
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[SearchMessageByTime] Send a request, requestId = " << pbReq.requestId()
              << ", loginSessionId=" << loginSessionId << ", chatSessionId=" << chatSessionId
              << ", begTime=" << begTime << ", endTime=" << endTime;

        QNetworkReply* resp = this->SendHttpRequest("/service/message_storage/get_history", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::GetHistoryMsgRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[SearchMessageByTime] Error, reason=" << reason;
                return;
            }

            dataCenter->ResetSearchMessageResult(pbResp->msgList());
            emit dataCenter->SearchMessageDone();

            LOG() << "[SearchMessageByTime] Process the response done, requestId = " << pbResp->requestId();
        });
    }

    void NetClient::UserLogin(const QString &username, const QString &password)
    {
        SnowK::UserLoginReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setNickname(username);
        pbReq.setPassword(password);
        pbReq.setVerifyCodeId("");
        pbReq.setVerifyCode("");
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[UserLogin] Send a request, requestId = " << pbReq.requestId()
              << ", username=" << pbReq.nickname() << ", password=" << pbReq.password();

        QNetworkReply* resp = this->SendHttpRequest("/service/user/username_login", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::UserLoginRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[UserLogin] Error, reason=" << reason;
                emit dataCenter->UserLoginDone(false, reason);
                return;
            }

            dataCenter->ResetLoginSessionId(pbResp->loginSessionId());
            emit dataCenter->UserLoginDone(true, "");

            LOG() << "[UserLogin] Process the response done, requestId = " << pbResp->requestId();
        });
    }

    void NetClient::UserRegister(const QString &username, const QString &password)
    {
        SnowK::UserRegisterReq pbReq;
        pbReq.setRequestId(MakeRequestId());
        pbReq.setNickname(username);
        pbReq.setPassword(password);
        pbReq.setVerifyCodeId("");
        pbReq.setVerifyCode("");
        QByteArray body = pbReq.serialize(&serializer);

        LOG() << "[UserRegister] Send a request, requestId = " << pbReq.requestId()
              << ", username=" << pbReq.nickname() << ", password=" << pbReq.password();

        QNetworkReply* resp = this->SendHttpRequest("/service/user/username_register", body);

        connect(resp, &QNetworkReply::finished, this, [=]()
        {
            bool ok = false;
            QString reason;
            auto pbResp = this->HandleHttpResponse<SnowK::UserRegisterRsp>(resp, &ok, &reason);

            if (!ok)
            {
                LOG() << "[UserRegister] Error, reason=" << reason;
                emit dataCenter->UserRegisterDone(false, reason);
                return;
            }

            emit dataCenter->UserRegisterDone(true, "");

            LOG() << "[UserRegister] Process the response done, requestId = " << pbResp->requestId();
        });
    }

    void NetClient::PhoneLogin(const QString &phone, const QString &verifyCodeId, const QString &verifyCode)
    {

    }

    void NetClient::PhoneRegister(const QString &phone, const QString &verifyCodeId, const QString &verifyCode)
    {

    }

} // end of namespace network
