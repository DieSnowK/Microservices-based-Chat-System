#include "server.h"

namespace Util
{
    static inline QString GetFileName(const QString& path)
    {
        return QFileInfo(path).fileName();
    }

    #define TAG QString("[%1:%2]").arg(Util::GetFileName(__FILE__), QString::number(__LINE__))
    #define LOG() qDebug().noquote() << TAG

    static inline QString FormatTime(int64_t timestamp)
    {
        return QDateTime(QDateTime::fromSecsSinceEpoch(timestamp)).toString("MM-dd HH:mm:ss");
    }

    static inline int64_t GetTime()
    {
        return QDateTime::currentSecsSinceEpoch();
    }

    static inline QIcon MakeIcon(const QByteArray& byteArray)
    {
        QPixmap pixmap;
        pixmap.loadFromData(byteArray);

        return QIcon(pixmap);
    }

    static inline QByteArray LoadFileToByteArray(const QString& path)
    {
        QFile file(path);
        if(!file.open(QFile::ReadOnly))
        {
            LOG() << "Failed to open file";
            return QByteArray();
        }

        QByteArray content = file.readAll();

        file.close();
        return content;
    }

    static inline void WriteByteArrayToFile(const QString& path, const QByteArray& content)
    {
        QFile file(path);
        if(!file.open(QFile::WriteOnly))
        {
            LOG() << "Failed to open file";
            return;
        }

        file.write(content);
        file.flush();

        file.close();
    }
} // end of namespace Util

//////////////////////////////////////////////////////////////////
/// Helper functions
//////////////////////////////////////////////////////////////////

SnowK::UserInfo MakeUserInfo(int index, const QByteArray& avatar)
{
    SnowK::UserInfo userInfo;
    userInfo.setUserId(QString::number(1000 + index));
    userInfo.setNickname("DieSnowK" + QString::number(index));
    userInfo.setDescription("Handsome Boy~" + QString::number(index));
    userInfo.setPhone("18312345678");
    userInfo.setAvatar(avatar);

    return userInfo;
}

SnowK::MessageInfo MakeTextMessageInfo(int index, const QString& chatSessionId, const QByteArray& avatar)
{
    SnowK::MessageInfo messageInfo;
    messageInfo.setMessageId(QString::number(3000 + index));
    messageInfo.setChatSessionId(chatSessionId);
    messageInfo.setTimestamp(Util::GetTime());
    messageInfo.setSender(MakeUserInfo(index, avatar));

    SnowK::StringMessageInfo stringMessageInfo;
    stringMessageInfo.setContent("This is a test message" + QString::number(index));

    SnowK::MessageContent messageContent;
    messageContent.setMessageType(SnowK::MessageTypeGadget::MessageType::STRING);
    messageContent.setStringMessage(stringMessageInfo);
    messageInfo.setMessage(messageContent);

    return messageInfo;
}

SnowK::MessageInfo MakeImageMessageInfo(int index, const QString& chatSessionId,
                                        const QByteArray& avatar)
{
    SnowK::MessageInfo messageInfo;
    messageInfo.setMessageId(QString::number(3000 + index));
    messageInfo.setChatSessionId(chatSessionId);
    messageInfo.setTimestamp(Util::GetTime());
    messageInfo.setSender(MakeUserInfo(index, avatar));

    SnowK::ImageMessageInfo imageMessageInfo;
    imageMessageInfo.setFileId("testImage");

    // No setup is required, it needs to be downloaded separately
    // imageMessageInfo.setImageContent();

    SnowK::MessageContent messageContent;
    messageContent.setMessageType(SnowK::MessageTypeGadget::MessageType::IMAGE);
    messageContent.setImageMessage(imageMessageInfo);
    messageInfo.setMessage(messageContent);
    return messageInfo;
}

SnowK::MessageInfo MakeFileMessageInfo(int index, const QString& chatSessionId,
                                       const QByteArray& avatar)
{
    SnowK::MessageInfo messageInfo;
    messageInfo.setMessageId(QString::number(3000 + index));
    messageInfo.setChatSessionId(chatSessionId);
    messageInfo.setTimestamp(Util::GetTime());
    messageInfo.setSender(MakeUserInfo(index, avatar));

    SnowK::FileMessageInfo fileMessageInfo;
    fileMessageInfo.setFileId("testFile");
    fileMessageInfo.setFileName("test.txt");
    fileMessageInfo.setFileSize(0);

    SnowK::MessageContent messageContent;
    messageContent.setMessageType(SnowK::MessageTypeGadget::MessageType::FILE);
    messageContent.setFileMessage(fileMessageInfo);
    messageInfo.setMessage(messageContent);
    return messageInfo;
}

SnowK::MessageInfo MakeSpeechMessageInfo(int index, const QString& chatSessionId,
                                         const QByteArray& avatar)
{
    SnowK::MessageInfo messageInfo;
    messageInfo.setMessageId(QString::number(3000 + index));
    messageInfo.setChatSessionId(chatSessionId);
    messageInfo.setTimestamp(Util::GetTime());
    messageInfo.setSender(MakeUserInfo(index, avatar));

    SnowK::SpeechMessageInfo speechMessageInfo;
    speechMessageInfo.setFileId("testSpeech");

    SnowK::MessageContent messageContent;
    messageContent.setMessageType(SnowK::MessageTypeGadget::MessageType::SPEECH);
    messageContent.setSpeechMessage(speechMessageInfo);
    messageInfo.setMessage(messageContent);
    return messageInfo;
}

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

    httpServer.route("/service/user/get_user_info", [=](const QHttpServerRequest& req)
    {
        return this->GetUserInfo(req);
    });

    httpServer.route("/service/friend/get_friend_list", [=](const QHttpServerRequest& req)
    {
        return this->GetFriendList(req);
    });

    httpServer.route("/service/friend/get_chat_session_list", [=](const QHttpServerRequest& req)
    {
        return this->GetChatSessionList(req);
    });

    httpServer.route("/service/friend/get_pending_friend_events", [=](const QHttpServerRequest& req)
    {
        return this->GetApplyList(req);
    });

    httpServer.route("/service/message_storage/get_recent", [=](const QHttpServerRequest& req)
    {
        return this->GetRecent(req);
    });

    httpServer.route("/service/message_transmit/new_message", [=](const QHttpServerRequest& req)
    {
        return this->NewMessage(req);
    });

    httpServer.route("/service/user/set_nickname", [=](const QHttpServerRequest& req)
    {
        return this->SetNickname(req);
    });

    httpServer.route("/service/user/set_description", [=](const QHttpServerRequest& req)
    {
        return this->SetDesc(req);
    });

    httpServer.route("/service/user/get_phone_verify_code", [=](const QHttpServerRequest& req) {
        return this->GetPhoneVerifyCode(req);
    });

    httpServer.route("/service/user/set_phone", [=](const QHttpServerRequest& req) {
        return this->SetPhone(req);
    });

    httpServer.route("/service/user/set_avatar", [=](const QHttpServerRequest& req) {
        return this->SetAvatar(req);
    });

    httpServer.route("/service/friend/remove_friend", [=](const QHttpServerRequest& req) {
        return this->RemoveFriend(req);
    });

    httpServer.route("/service/friend/add_friend_apply", [=](const QHttpServerRequest& req) {
        return this->AddFriendApply(req);
    });

    httpServer.route("/service/friend/add_friend_process", [=](const QHttpServerRequest& req) {
        return this->AddFriendProcess(req);
    });

    httpServer.route("/service/friend/create_chat_session", [=](const QHttpServerRequest& req)
    {
        return this->CreateChatSession(req);
    });

    httpServer.route("/service/friend/get_chat_session_member", [=](const QHttpServerRequest& req) {
        return this->GetChatSessionMember(req);
    });

    httpServer.route("/service/friend/search_friend", [=](const QHttpServerRequest& req) {
        return this->SearchFriend(req);
    });

    httpServer.route("/service/message_storage/search_history", [=](const QHttpServerRequest& req) {
        return this->SearchHistory(req);
    });

    httpServer.route("/service/message_storage/get_history", [=](const QHttpServerRequest& req) {
        return this->GetHistory(req);
    });

    httpServer.route("/service/user/username_login", [=](const QHttpServerRequest& req) {
        return this->UsernameLogin(req);
    });

    httpServer.route("/service/user/username_register", [=](const QHttpServerRequest& req) {
        return this->UsernameRegister(req);
    });

    httpServer.route("/service/user/phone_login", [=](const QHttpServerRequest& req) {
        return this->PhoneLogin(req);
    });

    httpServer.route("/service/user/phone_register", [=](const QHttpServerRequest& req) {
        return this->PhoneRegister(req);
    });

    return tcpServer.listen(QHostAddress::Any, 9000) && httpServer.bind(&tcpServer);
}

QHttpServerResponse HttpServer::GetUserInfo(const QHttpServerRequest &req)
{
    SnowK::GetUserInfoReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ GetUserInfo] requestId=" << pbReq.requestId()
          << ", loginSessionId=" << pbReq.sessionId();

    SnowK::GetUserInfoRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    SnowK::UserInfo userInfo;
    userInfo.setUserId("1024");
    userInfo.setNickname("DieSnowK");
    userInfo.setDescription("Handsome Boy~");
    userInfo.setPhone("18312345678");
    userInfo.setAvatar(Util::LoadFileToByteArray(":/resource/image/groupAvatar.png"));
    pbResp.setUserInfo(userInfo);

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::GetFriendList(const QHttpServerRequest &req)
{
    SnowK::GetFriendListReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ GetFriendList] requestId = "<< pbReq.requestId()
          << ", loginSessionId = " << pbReq.sessionId();

    SnowK::GetFriendListRsp pbRsp;
    pbRsp.setRequestId(pbReq.requestId());
    pbRsp.setSuccess(true);
    pbRsp.setErrmsg("");

    QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
    QList<SnowK::UserInfo> friendList;
    for (int i = 0; i < 20; ++i)
    {
        SnowK::UserInfo userInfo = MakeUserInfo(i, avatar);
        friendList.push_back(userInfo);
    }
    pbRsp.setFriendList(friendList);

    QByteArray body = pbRsp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::GetChatSessionList(const QHttpServerRequest &req)
{
    SnowK::GetChatSessionListReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ GetChatSessionList] requestId = "
          << pbReq.requestId() << ", loginSessionId = " << pbReq.sessionId();

    SnowK::GetChatSessionListRsp pbRsp;
    pbRsp.setRequestId(pbReq.requestId());
    pbRsp.setSuccess(true);
    pbRsp.setErrmsg("");

    QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
    QList<SnowK::ChatSessionInfo> chatSessionInfoList;
    for (int i = 0; i < 30; ++i)
    {
        SnowK::ChatSessionInfo chatSessionInfo;
        chatSessionInfo.setChatSessionId(QString::number(2000 + i));
        chatSessionInfo.setChatSessionName("ChatSession" + QString::number(i));
        chatSessionInfo.setSingleChatFriendId(QString::number(1000 + i));
        chatSessionInfo.setAvatar(avatar);

        SnowK::MessageInfo messageInfo = MakeTextMessageInfo(i, chatSessionInfo.chatSessionId(), avatar);
        chatSessionInfo.setPrevMessage(messageInfo);

        chatSessionInfoList.push_back(chatSessionInfo);
    }

    QByteArray groupAvatar = Util::LoadFileToByteArray(":/resource/image/groupAvatar.png");
    SnowK::ChatSessionInfo chatSessionInfo;
    chatSessionInfo.setChatSessionId(QString::number(2100));
    chatSessionInfo.setChatSessionName("ChatSession" + QString::number(2100));
    chatSessionInfo.setSingleChatFriendId("");
    chatSessionInfo.setAvatar(groupAvatar);
    SnowK::MessageInfo messageInfo = MakeTextMessageInfo(0, chatSessionInfo.chatSessionId(), avatar);
    chatSessionInfo.setPrevMessage(messageInfo);
    chatSessionInfoList.push_back(chatSessionInfo);

    pbRsp.setChatSessionInfoList(chatSessionInfoList);

    QByteArray body = pbRsp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::GetApplyList(const QHttpServerRequest &req)
{
    SnowK::GetPendingFriendEventListReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ GetApplyList] requestId = " << pbReq.requestId()
          << ", loginSessionId=" << pbReq.sessionId();

    SnowK::GetPendingFriendEventListRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
    QList<SnowK::FriendEvent> friendEventList;
    for (int i = 0; i < 5; ++i)
    {
        SnowK::FriendEvent friendEvent;
        friendEvent.setEventId("");
        friendEvent.setSender(MakeUserInfo(i, avatar));

        friendEventList.push_back(friendEvent);
    }
    pbResp.setEvent(friendEventList);

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::GetRecent(const QHttpServerRequest &req)
{
    SnowK::GetRecentMsgReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ GetRecent] requestId = " << pbReq.requestId()
          << ", loginSessionId = " << pbReq.sessionId()
          << ", chatSessionId = " << pbReq.chatSessionId();

    SnowK::GetRecentMsgRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
    QList<SnowK::MessageInfo> messageInfoList;
    for (int i = 0; i < 30; ++i)
    {
        SnowK::MessageInfo messageInfo = MakeTextMessageInfo(i, "2000", avatar);
        messageInfoList.push_back(messageInfo);
    }
    SnowK::MessageInfo imageMessageInfo = MakeImageMessageInfo(30, "2000", avatar);
    messageInfoList.push_back(imageMessageInfo);
    SnowK::MessageInfo fileMessageInfo = MakeFileMessageInfo(31, "2000", avatar);
    messageInfoList.push_back(fileMessageInfo);
    SnowK::MessageInfo speechMessageInfo = MakeSpeechMessageInfo(32, "2000", avatar);
    messageInfoList.push_back(speechMessageInfo);

    pbResp.setMsgList(messageInfoList);

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::NewMessage(const QHttpServerRequest &req)
{
    SnowK::NewMessageReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ NewMessage] requestId = " << pbReq.requestId()
          << ", loginSessionId = " << pbReq.sessionId()
          << ", chatSessionId = " << pbReq.chatSessionId()
          << ", messageType = " << pbReq.message().messageType();

    if (pbReq.message().messageType() == SnowK::MessageTypeGadget::MessageType::STRING)
    {
        LOG() << "NewMessage: =" << pbReq.message().stringMessage().content();
    }

    SnowK::NewMessageRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::SetNickname(const QHttpServerRequest &req)
{
    SnowK::SetUserNicknameReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ SetNickname] requestId = " << pbReq.requestId() << ", loginSessionId="
          << pbReq.sessionId() << ", nickname = " << pbReq.nickname();

    SnowK::SetUserNicknameRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::SetDesc(const QHttpServerRequest &req)
{
    SnowK::SetUserDescriptionReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ SetDesc] requestId=" << pbReq.requestId() << ", loginSessionId="
          << pbReq.sessionId() << ", desc=" << pbReq.description();

    SnowK::SetUserDescriptionRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::GetPhoneVerifyCode(const QHttpServerRequest &req)
{
    SnowK::PhoneVerifyCodeReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ GetPhoneVerifyCode] requestId=" << pbReq.requestId() << ", phone=" << pbReq.phoneNumber();

    SnowK::PhoneVerifyCodeRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");
    pbResp.setVerifyCodeId("testVerifyCodeId");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::SetPhone(const QHttpServerRequest &req)
{
    SnowK::SetUserPhoneNumberReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ SetPhone] requestId=" << pbReq.requestId() << ", loginSessionId="
          << pbReq.sessionId() << ", phone=" << pbReq.phoneNumber() << ", verifyCodeId="
          << pbReq.phoneVerifyCodeId() << ", verifyCode=" << pbReq.phoneVerifyCode();

    SnowK::SetUserPhoneNumberRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::SetAvatar(const QHttpServerRequest &req)
{
    SnowK::SetUserAvatarReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ SetAvatar] requestId = " << pbReq.requestId() << ", loginSessionId=" << pbReq.sessionId();

    SnowK::SetUserAvatarRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::RemoveFriend(const QHttpServerRequest &req)
{
    SnowK::FriendRemoveReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ RemoveFriend] requestId = " << pbReq.requestId() << ", loginSessionId="
          << pbReq.sessionId() << ", peerId=" << pbReq.peerId();

    SnowK::FriendRemoveRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::AddFriendApply(const QHttpServerRequest &req)
{
    SnowK::FriendAddReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ AddFriendApply] requestId=" << pbReq.requestId() << ", loginSessionId="
          << pbReq.sessionId() << ", userId=" << pbReq.respondentId();

    SnowK::FriendAddRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");
    pbResp.setNotifyEventId("");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::AddFriendProcess(const QHttpServerRequest &req)
{
    SnowK::FriendAddProcessReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ AddFriendProcess] requestId=" << pbReq.requestId() << ", loginSessionId="
          << pbReq.sessionId() << ", applyUserId=" << pbReq.applyUserId() << ", agree=" << pbReq.agree();

    SnowK::FriendAddProcessRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");
    pbResp.setNewSessionId("");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::CreateChatSession(const QHttpServerRequest &req)
{
    SnowK::ChatSessionCreateReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ CreateChatSession] requestId=" << pbReq.requestId() << ", loginSessionId="
          << pbReq.sessionId() << ", userIdList=" << pbReq.memberIdList();

    SnowK::ChatSessionCreateRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::GetChatSessionMember(const QHttpServerRequest &req)
{
    SnowK::GetChatSessionMemberReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ GetChatSessionMember] requestId=" << pbReq.requestId() << ", loginSessionId="
          << pbReq.sessionId() << ", chatSessionId=" << pbReq.chatSessionId();

    SnowK::GetChatSessionMemberRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
    QList<SnowK::UserInfo> userInfoList;
    for (int i = 0; i < 10; ++i)
    {
        SnowK::UserInfo userInfo = MakeUserInfo(i, avatar);
        userInfoList.push_back(userInfo);
    }
    pbResp.setMemberInfoList(userInfoList);

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::SearchFriend(const QHttpServerRequest &req)
{
    SnowK::FriendSearchReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ SearchFriend] requestId=" << pbReq.requestId() << ", loginSessionId="
          << pbReq.sessionId() << ", searchKey=" << pbReq.searchKey();

    SnowK::FriendSearchRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
    QList<SnowK::UserInfo> userInfoList;
    for (int i = 0; i < 30; ++i)
    {
        SnowK::UserInfo userInfo = MakeUserInfo(i, avatar);
        userInfoList.push_back(userInfo);
    }
    pbResp.setUserInfo(userInfoList);

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::SearchHistory(const QHttpServerRequest &req)
{
    SnowK::MsgSearchReq pbReq;
    pbReq.deserialize(&serializer, req.body());

    LOG() << "[REQ SearchHistory] requestId=" << pbReq.requestId() << ", loginSessionId=" << pbReq.sessionId()
          << ", chatSessionId=" << pbReq.chatSessionId() << ", searchKey=" << pbReq.searchKey();

    SnowK::MsgSearchRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
    QList<SnowK::MessageInfo> msgList;
    for (int i = 0; i < 10; ++i)
    {
        SnowK::MessageInfo message = MakeTextMessageInfo(i, pbReq.chatSessionId(), avatar);
        msgList.push_back(message);
    }
    SnowK::MessageInfo message = MakeImageMessageInfo(10, pbReq.chatSessionId(), avatar);
    msgList.push_back(message);
    message = MakeFileMessageInfo(11, pbReq.chatSessionId(), avatar);
    msgList.push_back(message);
    message = MakeSpeechMessageInfo(12, pbReq.chatSessionId(), avatar);
    msgList.push_back(message);

    pbResp.setMsgList(msgList);

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::GetHistory(const QHttpServerRequest &req)
{
    SnowK::GetHistoryMsgReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ GetHistory] requestId=" << pbReq.requestId() << ", loginSessionId="
          << pbReq.sessionId() << ", chatSessionId=" << pbReq.chatSessionId()
          << ", begTime=" << pbReq.startTime() << ", endTime=" << pbReq.overTime();

    SnowK::GetHistoryMsgRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
    QList<SnowK::MessageInfo> msgList;
    for (int i = 0; i < 10; ++i)
    {
        SnowK::MessageInfo message = MakeTextMessageInfo(i, pbReq.chatSessionId(), avatar);
        msgList.push_back(message);
    }
    pbResp.setMsgList(msgList);

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::UsernameLogin(const QHttpServerRequest &req)
{
    SnowK::UserLoginReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ UsernameLogin] requestId=" << pbReq.requestId() << ", username="
          << pbReq.nickname() << ", password=" << pbReq.password();

    SnowK::UserLoginRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");
    pbResp.setLoginSessionId("testLoginSessionId");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::UsernameRegister(const QHttpServerRequest &req)
{
    SnowK::UserRegisterReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ UsernameRegister] requestId=" << pbReq.requestId() << ", username="
          << pbReq.nickname() << ", password=" << pbReq.password();

    SnowK::UserRegisterRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QString body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::PhoneLogin(const QHttpServerRequest &req)
{
    SnowK::PhoneLoginReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ PhoneLogin] requestId=" << pbReq.requestId() << ", phone=" << pbReq.phoneNumber()
          << ", verifyCodeId=" << pbReq.verifyCodeId() << ", verifyCode=" << pbReq.verifyCode();

    SnowK::PhoneLoginRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");
    pbResp.setLoginSessionId("testLoginSessionId");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
}

QHttpServerResponse HttpServer::PhoneRegister(const QHttpServerRequest &req)
{
    SnowK::PhoneRegisterReq pbReq;
    pbReq.deserialize(&serializer, req.body());
    LOG() << "[REQ PhoneRegister] requestId=" << pbReq.requestId() << ", phone=" << pbReq.phoneNumber()
          << ", verifyCodeId=" << pbReq.verifyCodeId() << ", verifyCode=" << pbReq.verifyCode();

    SnowK::PhoneRegisterRsp pbResp;
    pbResp.setRequestId(pbReq.requestId());
    pbResp.setSuccess(true);
    pbResp.setErrmsg("");

    QByteArray body = pbResp.serialize(&serializer);
    QHttpServerResponse httpResp(body, QHttpServerResponse::StatusCode::Ok);

    QHttpHeaders httpHeader;
    httpHeader.append(QHttpHeaders::WellKnownHeader::ContentType, "application/x-protobuf");
    httpResp.setHeaders(httpHeader);

    return httpResp;
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

            disconnect(this, &WebsocketServer::SendTextResp, this, nullptr);
            disconnect(this, &WebsocketServer::SendFriendRemove, this, nullptr);
            disconnect(this, &WebsocketServer::SendAddFriendApply, this, nullptr);
            disconnect(this, &WebsocketServer::SendAddFriendProcess, this, nullptr);
            disconnect(this, &WebsocketServer::SendCreateChatSession, this, nullptr);
            disconnect(this, &WebsocketServer::SendImageResp, this, nullptr);
            disconnect(this, &WebsocketServer::SendFileResp, this, nullptr);
            disconnect(this, &WebsocketServer::SendSpeechResp, this, nullptr);
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

        connect(this, &WebsocketServer::SendTextResp, this, [=]()
        {
            if (socket == nullptr || !socket->isValid())
            {
                LOG() << "The socket object is invalid";
                return;
            }

            QByteArray avatar = Util::LoadFileToByteArray(":/resouce/image/defaultAvatar.png");
            SnowK::MessageInfo messageInfo = MakeTextMessageInfo(this->messageIndex++, "2000", avatar);

            SnowK::NotifyNewMessage notifyNewMessage;
            notifyNewMessage.setMessageInfo(messageInfo);

            SnowK::NotifyMessage notifyMessage;
            notifyMessage.setNotifyEventId("");
            notifyMessage.setNotifyType(SnowK::NotifyTypeGadget::NotifyType::CHAT_MESSAGE_NOTIFY);
            notifyMessage.setNewMessageInfo(notifyNewMessage);

            QByteArray body = notifyMessage.serialize(&this->serializer);
            socket->sendBinaryMessage(body);

            LOG() << "SendTextResp: " << messageInfo.message().stringMessage().content();
        });

        connect(this, &WebsocketServer::SendImageResp, this, [=]()
        {
            if (socket == nullptr || !socket->isValid())
            {
                LOG() << "The socket object is invalid";
                return;
            }

            QByteArray avatar = Util::LoadFileToByteArray(":/resouce/image/defaultAvatar.png");
            SnowK::MessageInfo messageInfo = MakeImageMessageInfo(this->messageIndex++, "2000", avatar);

            SnowK::NotifyNewMessage notifyNewMessage;
            notifyNewMessage.setMessageInfo(messageInfo);

            SnowK::NotifyMessage notifyMessage;
            notifyMessage.setNotifyEventId("");
            notifyMessage.setNotifyType(SnowK::NotifyTypeGadget::NotifyType::CHAT_MESSAGE_NOTIFY);
            notifyMessage.setNewMessageInfo(notifyNewMessage);

            QByteArray body = notifyMessage.serialize(&this->serializer);
            socket->sendBinaryMessage(body);

            LOG() << "SendImageResp";
        });

        connect(this, &WebsocketServer::SendFileResp, this, [=]()
        {
            if (socket == nullptr || !socket->isValid())
            {
                LOG() << "The socket object is invalid";
                return;
            }

            QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
            SnowK::MessageInfo messageInfo = MakeFileMessageInfo(this->messageIndex++, "2000", avatar);

            SnowK::NotifyNewMessage notifyNewMessage;
            notifyNewMessage.setMessageInfo(messageInfo);

            SnowK::NotifyMessage notifyMessage;
            notifyMessage.setNotifyEventId("");
            notifyMessage.setNotifyType(SnowK::NotifyTypeGadget::NotifyType::CHAT_MESSAGE_NOTIFY);
            notifyMessage.setNewMessageInfo(notifyNewMessage);

            QByteArray body = notifyMessage.serialize(&this->serializer);
            socket->sendBinaryMessage(body);

            LOG() << "SendFileResp";
        });

        connect(this, &WebsocketServer::SendSpeechResp, this, [=]()
        {
            if (socket == nullptr || !socket->isValid())
            {
                LOG() << "The socket object is invalid";
                return;
            }

            QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
            SnowK::MessageInfo messageInfo = MakeSpeechMessageInfo(this->messageIndex++, "2000", avatar);

            SnowK::NotifyNewMessage notifyNewMessage;
            notifyNewMessage.setMessageInfo(messageInfo);

            SnowK::NotifyMessage notifyMessage;
            notifyMessage.setNotifyEventId("");
            notifyMessage.setNotifyType(SnowK::NotifyTypeGadget::NotifyType::CHAT_MESSAGE_NOTIFY);
            notifyMessage.setNewMessageInfo(notifyNewMessage);

            QByteArray body = notifyMessage.serialize(&this->serializer);
            socket->sendBinaryMessage(body);

            LOG() << "SendSpeechResp";
        });

        connect(this, &WebsocketServer::SendFriendRemove, this, [=]()
        {
            if (socket == nullptr || !socket->isValid()) {
                LOG() << "The socket object is invalid";
                return;
            }

            SnowK::NotifyMessage notifyMessage;
            notifyMessage.setNotifyEventId("");
            notifyMessage.setNotifyType(SnowK::NotifyTypeGadget::NotifyType::FRIEND_REMOVE_NOTIFY);

            SnowK::NotifyFriendRemove notifyFriendRemove;
            notifyFriendRemove.setUserId("1000");
            notifyMessage.setFriendRemove(notifyFriendRemove);

            QByteArray body = notifyMessage.serialize(&serializer);
            socket->sendBinaryMessage(body);

            LOG() << "SendFriendRemove, userId=1000";
        });

        connect(this, &WebsocketServer::SendAddFriendApply, this, [=]()
        {
            if (socket == nullptr || !socket->isValid())
            {
                LOG() << "The socket object is invalid";
                return;
            }

            SnowK::NotifyMessage notifyMessage;
            notifyMessage.setNotifyEventId("");
            notifyMessage.setNotifyType(SnowK::NotifyTypeGadget::NotifyType::FRIEND_ADD_APPLY_NOTIFY);

            QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
            SnowK::UserInfo userInfo = MakeUserInfo(100, avatar);

            SnowK::NotifyFriendAddApply friendAddApply;
            friendAddApply.setUserInfo(userInfo);

            notifyMessage.setFriendAddApply(friendAddApply);

            QByteArray body = notifyMessage.serialize(&serializer);
            socket->sendBinaryMessage(body);

            LOG() << "SendAddFriendApply";
        });

        connect(this, &WebsocketServer::SendAddFriendProcess, this, [=](bool agree)
        {
            if (socket == nullptr || !socket->isValid())
            {
                LOG() << "The socket object is invalid";
                return;
            }

            SnowK::NotifyMessage notifyMessage;
            notifyMessage.setNotifyEventId("");
            notifyMessage.setNotifyType(SnowK::NotifyTypeGadget::NotifyType::FRIEND_ADD_PROCESS_NOTIFY);

            QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/defaultAvatar.png");
            SnowK::UserInfo userInfo = MakeUserInfo(100, avatar);

            SnowK::NotifyFriendAddProcess friendAddProcess;
            friendAddProcess.setUserInfo(userInfo);
            friendAddProcess.setAgree(agree);

            notifyMessage.setFriendProcessResult(friendAddProcess);

            QByteArray body = notifyMessage.serialize(&serializer);
            socket->sendBinaryMessage(body);

            LOG() << "SendAddFriendProcess, userId=" << userInfo.userId() << ", agree=" << agree;
        });

        connect(this, &WebsocketServer::SendCreateChatSession, this, [=]()
        {
            if (socket == nullptr || !socket->isValid())
            {
                LOG() << "The socket object is invalid";
                return;
            }

            QByteArray avatar = Util::LoadFileToByteArray(":/resource/image/groupAvatar.png");

            SnowK::NotifyMessage notifyMessage;
            notifyMessage.setNotifyEventId("");
            notifyMessage.setNotifyType(SnowK::NotifyTypeGadget::NotifyType::CHAT_SESSION_CREATE_NOTIFY);

            SnowK::MessageInfo messageInfo = MakeTextMessageInfo(0, "2100", avatar);

            SnowK::ChatSessionInfo chatSessionInfo;
            chatSessionInfo.setChatSessionId("2100");
            chatSessionInfo.setSingleChatFriendId("");
            chatSessionInfo.setChatSessionName("New group chats");
            chatSessionInfo.setPrevMessage(messageInfo);
            chatSessionInfo.setAvatar(avatar);

            SnowK::NotifyNewChatSession newChatSession;
            newChatSession.setChatSessionInfo(chatSessionInfo);
            notifyMessage.setNewChatSessionInfo(newChatSession);

            QByteArray body = notifyMessage.serialize(&serializer);
            socket->sendBinaryMessage(body);

            LOG() << "SendCreateChatSession";
        });
    });

    return websocketServer.listen(QHostAddress::Any, 9001);
}
