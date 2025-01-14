#ifndef DATACENTER_H
#define DATACENTER_H

#include <QObject>

#include "data.hpp"
#include "netclient.h"

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

        void InitWebsocket();

        const QString& GetLoginSessionId() const;
        UserInfo* GetMyself();
        QList<UserInfo>* GetFriendList();
        QList<ChatSessionInfo>* GetChatSessionList();
        QList<UserInfo>* GetApplyList();
        QList<Message>* GetRecentMessageList(const QString& chatSessionId);
        const QString& GetVerifyCodeId();
        QList<UserInfo>* GetMemberList(const QString& chatSessionId);
        QList<UserInfo>* GetSearchUserResult();
        QList<Message>* GetSearchMessageResult();

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

        void GetVerifyCodeAsync(const QString& phone);
        void ResetVerifyCodeId(const QString& verifyCodeId);

        void ChangePhoneAsync(const QString& phone, const QString& verifyCodeId, const QString& verifyCode);
        void ResetPhone(const QString& phone);

        void ChangeAvatarAsync(const QByteArray& imageBytes);
        void ResetAvatar(const QByteArray& avatar);

        void DeleteFriendAsync(const QString& userId);
        void RemoveFriend(const QString& userId);

        void AddFriendApplyAsync(const QString& userId);

        void AcceptFriendApplyAsync(const QString& userId);
        UserInfo RemoveFromApplyList(const QString& userId);

        void RejectFriendApplyAsync(const QString& userId);

        void CreateGroupChatSessionAsync(const QList<QString>& userIdList);

        void GetMemberListAsync(const QString& chatSessionId);
        void ResetMemberList(const QString& chatSessionId, const QList<SnowK::UserInfo>& memberList);

        void SearchUserAsync(const QString& searchKey);
        void ResetSearchUserResult(const QList<SnowK::UserInfo>& userList);

        void SearchMessageAsync(const QString& searchKey);
        void SearchMessageByTimeAsync(const QDateTime& begTime, const QDateTime& endTime);
        void ResetSearchMessageResult(const QList<SnowK::MessageInfo>& msgList);

        void UserLoginAsync(const QString& username, const QString& password);
        void ResetLoginSessionId(const QString& loginSessionId);
        void UserRegisterAsync(const QString& username, const QString& password);
        void PhoneLoginAsync(const QString& phone, const QString& verifyCode);
        void PhoneRegisterAsync(const QString& phone, const QString& verifyCode);

        void GetSingleFileAsync(const QString& fileId);

        void SpeechConvertTextAsync(const QString& fileId, const QByteArray& content);

        //////////////////////////////////////////////////////////////////
        /// Helper functions
        //////////////////////////////////////////////////////////////////
        ChatSessionInfo* FindChatSessionById(const QString& chatSessionId);
        ChatSessionInfo* FindChatSessionByUserId(const QString& userId);
        UserInfo* FindFriendById(const QString& userId);

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
        void DeleteFriendDone();
        void ClearCurrentSession();
        void AddFriendApplyDone();
        void ReceiveFriendApplyDone();
        void AcceptFriendApplyDone();
        void RejectFriendApplyDone();
        void ReceiveFriendProcessDone(const QString& nickname, bool agree);
        void CreateGroupChatSessionDone();
        void ReceiveSessionCreateDone();
        void GetMemberListDone(const QString& chatSessionId);
        void SearchUserDone();
        void SearchMessageDone();
        void UserLoginDone(bool ok, const QString& reason);
        void UserRegisterDone(bool ok, const QString& reason);
        void PhoneLoginDone(bool ok, const QString& reason);
        void PhoneRegisterDone(bool ok, const QString& reason);
        void GetSingleFileDone(const QString& fileId, const QByteArray& fileContent);
        void SpeechConvertTextDone(const QString& fileId, const QString& text);

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
