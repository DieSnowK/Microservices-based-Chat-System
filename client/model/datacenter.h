#ifndef DATACENTER_H
#define DATACENTER_H

#include <QObject>

#include "data.hpp"
#include "../network/netclient.h"

namespace model
{
    // DataCenter is only responsible for "processing the data" and actually
        // accessing the network for communication, which needs to go through NetClient
    class DataCenter : public QObject
    {
        Q_OBJECT

    public:
        ~DataCenter();
        static DataCenter* GetInstance();

        void InitDataFile();
        void SaveDataFile();
        void LoadDataFile();

        void Ping();

        const QString& GetLoginSessionId() const;
        UserInfo* GetMyself();
        QList<UserInfo>* GetFriendList();
        QList<ChatSessionInfo>* GetChatSessionList();

        void GetMyselfAsync();
        void ResetMyself(std::shared_ptr<SnowK::GetUserInfoRsp> resp);

        void GetFriendListAsync();
        void ResetFriendList(std::shared_ptr<SnowK::GetFriendListRsp> resp);

        void GetChatSessionListAsync();
        void ResetChatSessionList(std::shared_ptr<SnowK::GetChatSessionListRsp> resp);

        void GetApplyListAsync();

    signals:
        void GetMyselfDone();
        void GetFriendListDone();
        void GetChatSessionListDone();
        void GetApplyListDone();

    private:
        DataCenter();
        static DataCenter* instance;

        QString loginSessionId = "";
        UserInfo* myself = nullptr;
        QList<UserInfo>* friendList = nullptr;
        QList<ChatSessionInfo>* chatSessionList = nullptr;
        QString currentChatSessionId = "";
        
        // Record which members are in each session (mainly for group chats)
            // key is chatSessionId, and value is the list of members
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
