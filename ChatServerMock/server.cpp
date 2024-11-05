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


    return tcpServer.listen(QHostAddress::Any, 8000) && httpServer.bind(&tcpServer);
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
