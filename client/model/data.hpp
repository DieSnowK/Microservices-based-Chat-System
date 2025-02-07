#pragma once

#include <QString>
#include <QIcon>
#include <QUuid>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include "base.qpb.h"
#include "gateway.qpb.h"
#include "user.qpb.h"
#include "friend.qpb.h"
#include "file.qpb.h"
#include "notify.qpb.h"
#include "speech.qpb.h"
#include "message.qpb.h"
#include "transmite.qpb.h"

namespace model
{
    namespace Util
    {
        static inline QString GetFileName(const QString& path)
        {
            return QFileInfo(path).fileName();
        }

        #define TAG QString("[%1:%2]").arg(model::Util::GetFileName(__FILE__), QString::number(__LINE__))
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

    //////////////////////////////////////////////////////
    /// UserInfo
    //////////////////////////////////////////////////////

    class UserInfo
    {
    public:
        void Load(const SnowK::UserInfo& userInfo)
        {
            this->userId = userInfo.userId();
            this->nickname = userInfo.nickname();
            this->phone = userInfo.phone();
            this->description = userInfo.description();
            if (userInfo.avatar().isEmpty())
            {
                this->avatar = QIcon(":/resource/image/defaultAvatar.png");
            }
            else
            {
                this->avatar = Util::MakeIcon(userInfo.avatar());
            }
        }

    public:
        QString userId = "";
        QString nickname = "";
        QString description = "";
        QString phone = "";
        QIcon avatar;
    }; // end of class UserInfo

    //////////////////////////////////////////////////////
    /// Message
    //////////////////////////////////////////////////////

    enum class MessageType
    {
        TEXT_TYPE,
        IMAGE_TYPE,
        FILE_TYPE,
        SPEECH_TYPE
    };

    class Message
    {
    public:
        void Load(const SnowK::MessageInfo& messageInfo)
        {
            this->messageId = messageInfo.messageId();
            this->chatSessionId = messageInfo.chatSessionId();
            this->time = Util::FormatTime(messageInfo.timestamp());
            this->sender.Load(messageInfo.sender());

            auto type = messageInfo.message().messageType();
            if (type == SnowK::MessageTypeGadget::MessageType::STRING)
            {
                this->msgType = MessageType::TEXT_TYPE;
                this->content = messageInfo.message().stringMessage().content().toUtf8();
            }
            else if (type == SnowK::MessageTypeGadget::MessageType::IMAGE)
            {
                this->msgType = MessageType::IMAGE_TYPE;
                if (messageInfo.message().imageMessage().hasImageContent())
                {
                    this->content = messageInfo.message().imageMessage().imageContent();
                }
                if (messageInfo.message().imageMessage().hasFileId()) 
                {
                    this->fileId = messageInfo.message().imageMessage().fileId();
                }
            }
            else if (type == SnowK::MessageTypeGadget::MessageType::FILE)
            {
                this->msgType = MessageType::FILE_TYPE;
                if (messageInfo.message().fileMessage().hasFileContents()) 
                {
                    this->content = messageInfo.message().fileMessage().fileContents();
                }
                if (messageInfo.message().fileMessage().hasFileId()) 
                {
                    this->fileId = messageInfo.message().fileMessage().fileId();
                }
                this->fileName = messageInfo.message().fileMessage().fileName();
            }
            else if (type == SnowK::MessageTypeGadget::MessageType::SPEECH)
            {
                this->msgType = MessageType::SPEECH_TYPE;
                if (messageInfo.message().speechMessage().hasFileContents()) 
                {
                    this->content = messageInfo.message().speechMessage().fileContents();
                }
                if (messageInfo.message().speechMessage().hasFileId()) 
                {
                    this->fileId = messageInfo.message().speechMessage().fileId();
                }
            }
            else
            {
                LOG() << "Error MessageType, type = " << type;
            }
        }

        // Factory Pattern
        // The extraInfo here is currently only supplemented as a "filename"
            // when the message type is a file message
        static Message MakeMessage(MessageType msgType, const QString& chatSessionId, const UserInfo& sender,
                                   const QByteArray& content, const QString& extraInfo)
        {
            switch(msgType)
            {
            case MessageType::TEXT_TYPE:
                return MakeTextMessage(chatSessionId, sender, content);
            case MessageType::IMAGE_TYPE:
                return MakeImageMessage(chatSessionId, sender, content);
            case MessageType::FILE_TYPE:
                return MakeFileMessage(chatSessionId, sender, content, extraInfo);
            case MessageType::SPEECH_TYPE:
                return MakeSpeechMessage(chatSessionId, sender, content);
            default:
                return Message();
            }
        }

    private:
        static QString MakeId()
        {
            return "M" + QUuid::createUuid().toString().sliced(25, 12); 
        }

        static Message MakeTextMessage(const QString& chatSessionId, 
                                       const UserInfo& sender, 
                                       const QByteArray& content)
        {
            Message msg;
            msg.messageId = MakeId();
            msg.chatSessionId= chatSessionId;
            msg.sender = sender;
            msg.time = Util::FormatTime(Util::GetTime());
            msg.content = content;
            msg.msgType = MessageType::TEXT_TYPE;

            msg.fileId = "";
            msg.fileName = "";

            return msg;
        }

        static Message MakeImageMessage(const QString& chatSessionId, 
                                        const UserInfo& sender, 
                                        const QByteArray& content)
        {
            Message msg;
            msg.messageId = MakeId();
            msg.chatSessionId= chatSessionId;
            msg.sender = sender;
            msg.time = Util::FormatTime(Util::GetTime());
            msg.content = content;
            msg.msgType = MessageType::IMAGE_TYPE;

            msg.fileId = "";
            msg.fileName = "";

            return msg;
        }

        static Message MakeFileMessage(const QString& chatSessionId, const UserInfo& sender, 
                                       const QByteArray& content, const QString& fileName)
        {
            Message msg;
            msg.messageId = MakeId();
            msg.chatSessionId= chatSessionId;
            msg.sender = sender;
            msg.time = Util::FormatTime(Util::GetTime());
            msg.content = content;
            msg.msgType = MessageType::FILE_TYPE;

            msg.fileId = "";
            msg.fileName = fileName;

            return msg;
        }

        static Message MakeSpeechMessage(const QString& chatSessionId, 
                                         const UserInfo& sender, 
                                         const QByteArray& content)
        {
            Message msg;
            msg.messageId = MakeId();
            msg.chatSessionId= chatSessionId;
            msg.sender = sender;
            msg.time = Util::FormatTime(Util::GetTime());
            msg.content = content;
            msg.msgType = MessageType::SPEECH_TYPE;

            msg.fileId = "";
            msg.fileName = "";

            return msg;
        }

    public:
        QString messageId = "";
        QString chatSessionId = "";
        QString time = "";
        MessageType msgType = MessageType::TEXT_TYPE;
        UserInfo sender;
        QByteArray content;
        QString fileId = "";
        QString fileName = "";
    }; // end of class Message

    //////////////////////////////////////////////////////
    /// ChatSessionInfo
    //////////////////////////////////////////////////////

    class ChatSessionInfo
    {
    public:
        void Load(const SnowK::ChatSessionInfo& chatSessionInfo)
        {
            this->chatSessionId = chatSessionInfo.chatSessionId();
            this->chatSessionName = chatSessionInfo.chatSessionName();

            if (chatSessionInfo.hasSingleChatFriendId())
            {
                this->userId = chatSessionInfo.singleChatFriendId();
            }

            if (chatSessionInfo.hasPrevMessage())
            {
                lastMessage.Load(chatSessionInfo.prevMessage());
            }

            if (chatSessionInfo.hasAvatar() && !chatSessionInfo.avatar().isEmpty())
            {
                this->avatar = Util::MakeIcon(chatSessionInfo.avatar());
            }
            else
            {
                // If don't have an avatar, a different default avatar is used depending on
                    // whether the current session is a one-on-one chat or a group chat
                if (userId != "")
                {
                    this->avatar = QIcon(":/resource/image/defaultAvatar.png");
                }
                else
                {
                    this->avatar = QIcon(":/resource/image/groupAvatar.png");
                }
            }
        }

    public:
        QString chatSessionId = "";
        QString chatSessionName = "";
        Message lastMessage;
        QIcon avatar;
        QString userId = "";
    }; // end of ChatSessionInfo
} // end of namespae model
