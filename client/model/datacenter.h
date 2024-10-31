#ifndef DATACENTER_H
#define DATACENTER_H

#include <QObject>

#include "data.hpp"
#include "../network/netclient.h"

namespace model
{
    class DataCenter : public QObject
    {
        Q_OBJECT

    public:
        ~DataCenter();
        static DataCenter* GetInstance();

        void InitDataFile();
        void SaveDataFile();
        void LoadDataFile();

    private:
        DataCenter();
        static DataCenter* instance;

        QString loginSessionId = "";
        UserInfo* myself = nullptr;
        QList<UserInfo>* friendList = nullptr;
        QList<ChatSessionInfo>* chatSessionList = nullptr;
        QString currentChatSessionId = "";
        
        // 记录每个会话中, 都有哪些成员(主要针对群聊). key 为 chatSessionId, value 为成员列表
        QHash<QString, QList<UserInfo>>* memberList = nullptr;

        QList<UserInfo>* applyList = nullptr;
        QHash<QString, QList<Message>>* recentMessages = nullptr; // <chatSessionId, QList<Message>>
        QHash<QString, int>* unreadMessageCount = nullptr; // <chatSessionId, int>
        QList<UserInfo>* searchUserResult = nullptr;
        QList<Message>* searchMessageResult = nullptr;
        QString currentVerifyCodeId = "";

        network::NetClient netClient;
    };
} // end of namespace model

#endif // DATACENTER_H
