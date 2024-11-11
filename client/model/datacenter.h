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
        QList<UserInfo>* GetApplyList();
        QList<Message>* GetRecentMessageList(const QString& chatSessionId);

        //////////////////////////////////////////////////////////////////
        /// Core functions
        //////////////////////////////////////////////////////////////////

        void GetMyselfAsync();
        void ResetMyself(std::shared_ptr<SnowK::GetUserInfoRsp> resp);

        void GetFriendListAsync();
        void ResetFriendList(std::shared_ptr<SnowK::GetFriendListRsp> resp);

        void GetChatSessionListAsync();
        void ResetChatSessionList(std::shared_ptr<SnowK::GetChatSessionListRsp> resp);

        void GetApplyListAsync();
        void ResetApplyList(std::shared_ptr<SnowK::GetPendingFriendEventListRsp> resp);

        void GetRecentMessageListAsync(const QString& chatSessionId, bool updateUI);
        void ResetRecentMessageList(const QString& chatSessionId, std::shared_ptr<SnowK::GetRecentMsgRsp> resp);

        void SendTextMessageAsync(const QString& chatSessionId, const QString& content);
        void SendImageMessageAsync(const QString& chatSessionId, const QByteArray& content);
        void SendFileMessageAsync(const QString& chatSessionId, const QString& fileName, const QByteArray& content);
        void SendSpeechMessageAsync(const QString& chatSessionid, const QByteArray& content);

        void ChangeNicknameAsync(const QString& nickname);
        void ResetNickname(const QString& nickname);

        void ChangeDescriptionAsync(const QString& desc);
        void ResetDescription(const QString& desc);

        //////////////////////////////////////////////////////////////////
        /// Helper functions
        //////////////////////////////////////////////////////////////////
        ChatSessionInfo* FindChatSessionById(const QString& chatSessionId);
        ChatSessionInfo* FindChatSessionByUserId(const QString& userId);

        void TopChatSessionInfo(const ChatSessionInfo& chatSessionInfo);

        void SetCurrentChatSessionId(const QString& chatSessionId);
        const QString& GetCurrentChatSessionId();

        void AddMessage(const Message& message);

        void ClearUnread(const QString& chatSessionId);
        void AddUnread(const QString& chatSessionId);
        int GetUnread(const QString& chatSessionId);

    signals:
        void GetMyselfDone();
        void GetFriendListDone();
        void GetChatSessionListDone();
        void GetApplyListDone();
        void GetRecentMessageListDone(const QString& chatSessionId);
        void GetRecentMessageListDoneNoUI(const QString& chatSessionId);
        void SendMessageDone(MessageType messageType, const QByteArray& content, const QString& extraInfo);
        void UpdateLastMessage(const QString& chatSessionId);
        void ReceiveMessageDone(const Message& lastMessage);
        void ChangeNicknameDone();
        void ChangeDescriptionDone();
        void GetVerifyCodeDone();
        void ChangePhoneDone();
        void ChangeAvatarDone();

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
