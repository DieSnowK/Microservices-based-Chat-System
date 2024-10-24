#include <QString>
#include <QIcon>

namespace model
{
    class UserInfo
    {
        QString userId = "";
        QString nickname = "";
        QString description = "";
        QString phone = "";
        QIcon avatar;
    };

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
        QString messageId = "";
        QString chatSessionId = "";
        QString time = "";
        MessageType msgType = MessageType::TEXT_TYPE;
        UserInfo sender;
        QByteArray content;
        QString fileId = "";
        QString fileName = "";

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
        static Message MakeTextMessage(const QString& chatSessionId, 
                                       const UserInfo& sender, 
                                       const QByteArray& content)
        {

        }

        static Message MakeImageMessage(const QString& chatSessionId, 
                                        const UserInfo& sender, 
                                        const QByteArray& content)
        {

        }

        static Message MakeFileMessage(const QString& chatSessionId, const UserInfo& sender, 
                                       const QByteArray& content, const QString& filename)
        {

        }

        static Message MakeSpeechMessage(const QString& chatSessionId, 
                                         const UserInfo& sender, 
                                         const QByteArray& content)
        {

        }
    };
}
