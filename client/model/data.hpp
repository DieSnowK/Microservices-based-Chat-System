#pragma once

#include <QString>
#include <QIcon>
#include <QUuid>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

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
        QString userId = "";
        QString nickname = "";
        QString description = "";
        QString phone = "";
        QIcon avatar;
    };

    //////////////////////////////////////////////////////
    /// Message
    //////////////////////////////////////////////////////

    enum class MessageType
    {
        TEXT_TYPE,
        IMAGE_TYPE,
        FILE_TYPE,
        SPECCH_TYPE
    };

    class Message
    {
    public:
        // Factory Pattern
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
            case MessageType::SPECCH_TYPE:
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
            msg.msgType = MessageType::SPECCH_TYPE;

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
        QString chatSessionId = "";
        QString chatSessionName = "";
        Message lastMessage;
        QIcon avatar;
        QString userId = "";
    };
} // end of namespae model
