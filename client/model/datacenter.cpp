#include "datacenter.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>

#include "../debug.hpp"

namespace model
{
    DataCenter* DataCenter::instance = nullptr;

    DataCenter* DataCenter::GetInstance()
    {
        if(instance == nullptr)
        {
            instance = new DataCenter();
        }

        return instance;
    }

    DataCenter::DataCenter()
        : netClient(this)
    {
        // TODO
        recentMessages = new QHash<QString, QList<Message>>();
        memberList = new QHash<QString, QList<UserInfo>>();
        unreadMessageCount = new QHash<QString, int>();

        LoadDataFile();
    }

    DataCenter::~DataCenter()
    {
        delete myself;
        delete friendList;
        delete chatSessionList;
        delete memberList;
        delete applyList;
        delete recentMessages;
        delete unreadMessageCount;
        delete searchUserResult;
        delete searchMessageResult;
    }

    void DataCenter::InitDataFile()
    {
        QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QString filePath = basePath + "/ChatClient.json";

        QDir dir;
        if(!dir.exists(basePath))
        {
            dir.mkdir(basePath);
        }

        QFile file(filePath);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            LOG() << "Failed to open file: " << file.errorString();
            return;
        }

        QString data = "{\n\n}";
        file.write(data.toUtf8());

        file.close();
    }

    void DataCenter::SaveDataFile()
    {
        QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QString filePath = basePath + "/ChatClient.json";

        QFile file(filePath);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            LOG() << "Failed to open file" << file.errorString();
            return;
        }

        QJsonObject jsonObj;
        jsonObj["loginSessionId"] = loginSessionId;

        QJsonObject jsonUnread;
        for(auto it = unreadMessageCount->begin(); it != unreadMessageCount->end(); ++it)
        {
            jsonUnread[it.key()] = it.value();
        }
        jsonObj["unread"] = jsonUnread;
        
        QJsonDocument jsonDoc(jsonObj);
        QString str = jsonDoc.toJson();
        file.write(str.toUtf8());

        file.close();
    }

    void DataCenter::LoadDataFile()
    {
        QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QString filePath = basePath + "/ChatClient.json";
        
        QFileInfo fileInfo(filePath);
        if(!fileInfo.exists())
        {
            InitDataFile();
        }

        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            LOG() << "Failed to open file: " << file.errorString();
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
        if(jsonDoc.isNull())
        {
            LOG() << "Failed to parse ChatClient.json";
            file.close();
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        this->loginSessionId = jsonObj["loginSessionId"].toString();

        this->unreadMessageCount->clear();
        QJsonObject jsonUnread = jsonObj["unread"].toObject();
        for(auto it = jsonUnread.begin(); it != jsonUnread.end(); ++it)
        {
            this->unreadMessageCount->insert(it.key(), it.value().toInt());
        }

        file.close();
    }

    void DataCenter::Ping()
    {
        netClient.Ping();
    }

    void DataCenter::InitWebsocket()
    {
        netClient.InitWebsocket();
    }

    const QString &DataCenter::GetLoginSessionId() const
    {
        return loginSessionId;
    }

    UserInfo *DataCenter::GetMyself()
    {
        return myself;
    }

    QList<UserInfo> *DataCenter::GetFriendList()
    {
        return friendList;
    }

    QList<ChatSessionInfo> *DataCenter::GetChatSessionList()
    {
        return chatSessionList;
    }

    QList<UserInfo> *DataCenter::GetApplyList()
    {
        return applyList;
    }

    QList<Message> *DataCenter::GetRecentMessageList(const QString &chatSessionId)
    {
        if (!recentMessages->contains(chatSessionId))
        {
            return nullptr;
        }

        return &(*recentMessages)[chatSessionId];
    }

    const QString &DataCenter::GetVerifyCodeId()
    {
        return currentVerifyCodeId;
    }

    QList<UserInfo> *DataCenter::GetMemberList(const QString &chatSessionId)
    {
        if (!this->memberList->contains(chatSessionId))
        {
            return nullptr;
        }

        return &(*this->memberList)[chatSessionId];
    }

    QList<UserInfo> *DataCenter::GetSearchUserResult()
    {
        return searchUserResult;
    }

    QList<Message> *DataCenter::GetSearchMessageResult()
    {
        return searchMessageResult;
    }

    //////////////////////////////////////////////////////////////////
    /// Core functions
    //////////////////////////////////////////////////////////////////

    void DataCenter::GetMyselfAsync()
    {
        netClient.GetMyself(loginSessionId);
    }

    void DataCenter::ResetMyself(std::shared_ptr<SnowK::GetUserInfoRsp> resp)
    {
        if (myself == nullptr)
        {
            myself = new UserInfo();
        }

        const SnowK::UserInfo& userInfo = resp->userInfo();
        myself->Load(userInfo);
    }

    void DataCenter::GetFriendListAsync()
    {
        netClient.GetFriendList(loginSessionId);
    }

    void DataCenter::ResetFriendList(std::shared_ptr<SnowK::GetFriendListRsp> resp)
    {
        if (friendList == nullptr)
        {
            friendList = new QList<UserInfo>();
        }

        friendList->clear();

        auto& friendListPB = resp->friendList();
        for (auto& f : friendListPB)
        {
            UserInfo userInfo;
            userInfo.Load(f);
            friendList->push_back(userInfo);
        }
    }

    void DataCenter::GetChatSessionListAsync()
    {
        netClient.GetChatSessionList(loginSessionId);
    }

    void DataCenter::ResetChatSessionList(std::shared_ptr<SnowK::GetChatSessionListRsp> resp)
    {
        if (chatSessionList == nullptr)
        {
            chatSessionList = new QList<ChatSessionInfo>();
        }

        chatSessionList->clear();

        auto& chatSessionListPB = resp->chatSessionInfoList();
        for (auto& c : chatSessionListPB)
        {
            ChatSessionInfo chatSessionInfo;
            chatSessionInfo.Load(c);
            chatSessionList->push_back(chatSessionInfo);
        }
    }

    void DataCenter::GetApplyListAsync()
    {
        netClient.GetApplyList(loginSessionId);
    }

    void DataCenter::ResetApplyList(std::shared_ptr<SnowK::GetPendingFriendEventListRsp> resp)
    {
        if (applyList == nullptr)
        {
            applyList = new QList<UserInfo>();
        }

        applyList->clear();

        auto& eventList = resp->event();
        for (auto& event : eventList)
        {
            UserInfo userInfo;
            userInfo.Load(event.sender());
            applyList->push_back(userInfo);
        }
    }

    void DataCenter::GetRecentMessageListAsync(const QString &chatSessionId, bool updateUI)
    {
        netClient.GetRecentMessageList(loginSessionId, chatSessionId, updateUI);
    }

    void DataCenter::ResetRecentMessageList(const QString &chatSessionId, std::shared_ptr<SnowK::GetRecentMsgRsp> resp)
    {
        QList<Message>& messageList = (*recentMessages)[chatSessionId];
        messageList.clear();

        for (auto& m : resp->msgList())
        {
            Message message;
            message.Load(m);
            messageList.push_back(message);
        }
    }

    void DataCenter::SendTextMessageAsync(const QString &chatSessionId, const QString &content)
    {
        netClient.SendMessage(loginSessionId, chatSessionId,
                              MessageType::TEXT_TYPE, content.toUtf8(), "");
    }

    void DataCenter::SendImageMessageAsync(const QString &chatSessionId, const QByteArray &content)
    {
        netClient.SendMessage(loginSessionId, chatSessionId,
                              MessageType::IMAGE_TYPE, content, "");
    }

    void DataCenter::SendFileMessageAsync(const QString &chatSessionId, const QString &fileName, const QByteArray &content)
    {
        netClient.SendMessage(loginSessionId, chatSessionId,
                              MessageType::FILE_TYPE, content, fileName);
    }

    void DataCenter::SendSpeechMessageAsync(const QString &chatSessionid, const QByteArray &content)
    {
        netClient.SendMessage(loginSessionId, chatSessionid, MessageType::SPEECH_TYPE, content, "");
    }

    void DataCenter::ChangeNicknameAsync(const QString &nickname)
    {
        netClient.ChangeNickname(loginSessionId, nickname);
    }

    void DataCenter::ResetNickname(const QString &nickname)
    {
        if (myself == nullptr)
        {
            return;
        }
        myself->nickname = nickname;
    }

    void DataCenter::ChangeDescriptionAsync(const QString &desc)
    {
        netClient.ChangeDescription(loginSessionId, desc);
    }

    void DataCenter::ResetDescription(const QString &desc)
    {
        if (myself == nullptr)
        {
            return;
        }
        myself->description = desc;
    }

    void DataCenter::GetVerifyCodeAsync(const QString &phone)
    {
        // In order to be compatible with the logic of subsequent mobile
            // phone number login, the loginSessionId is not passed here.
        netClient.GetVerifyCode(phone);
    }

    void DataCenter::ResetVerifyCodeId(const QString &verifyCodeId)
    {
        this->currentVerifyCodeId = verifyCodeId;
    }

    void DataCenter::ChangePhoneAsync(const QString &phone, const QString &verifyCodeId, const QString &verifyCode)
    {
        netClient.ChangePhone(loginSessionId, phone, verifyCodeId, verifyCode);
    }

    void DataCenter::ResetPhone(const QString &phone)
    {
        if (myself == nullptr)
        {
            return;
        }
        myself->phone = phone;
    }

    void DataCenter::ChangeAvatarAsync(const QByteArray &imageBytes)
    {
        netClient.ChangeAvatar(loginSessionId, imageBytes);
    }

    void DataCenter::ResetAvatar(const QByteArray &avatar)
    {
        if (myself == nullptr)
        {
            return;
        }
        myself->avatar = Util::MakeIcon(avatar);
    }

    void DataCenter::DeleteFriendAsync(const QString &userId)
    {
        netClient.DeleteFriend(loginSessionId, userId);
    }

    void DataCenter::RemoveFriend(const QString &userId)
    {
        if (friendList == nullptr || chatSessionList == nullptr)
        {
            return;
        }

        friendList->removeIf([=](const UserInfo& userInfo) {
            return userInfo.userId == userId;
        });

        chatSessionList->removeIf([=](const ChatSessionInfo& chatSessionInfo)
        {
            if (chatSessionInfo.userId == "")
            {
                return false;
            }

            if (chatSessionInfo.userId == userId)
            {
                if (chatSessionInfo.chatSessionId == this->currentChatSessionId)
                {
                    emit this->ClearCurrentSession();
                }

                return true;
            }

            return false;
        });
    }

    void DataCenter::AddFriendApplyAsync(const QString &userId)
    {
        netClient.AddFriendApply(loginSessionId, userId);
    }

    void DataCenter::AcceptFriendApplyAsync(const QString &userId)
    {
        netClient.AcceptFriendApply(loginSessionId, userId);
    }

    // TODO memory leak?
    UserInfo DataCenter::RemoveFromApplyList(const QString &userId)
    {
        if (applyList == nullptr)
        {
            return UserInfo();
        }

        for (auto it = applyList->begin(); it != applyList->end(); ++it)
        {
            if (it->userId == userId)
            {
                UserInfo toDelete = *it;
                applyList->erase(it);
                return toDelete;
            }
        }

        return UserInfo();
    }

    void DataCenter::RejectFriendApplyAsync(const QString &userId)
    {
        netClient.RejectFriendApply(loginSessionId, userId);
    }

    void DataCenter::CreateGroupChatSessionAsync(const QList<QString> &userIdList)
    {
        netClient.CreateGroupChatSession(loginSessionId, userIdList);
    }

    void DataCenter::GetMemberListAsync(const QString &chatSessionId)
    {
        netClient.GetMemberList(loginSessionId, chatSessionId);
    }

    void DataCenter::ResetMemberList(const QString &chatSessionId, const QList<SnowK::UserInfo> &memberList)
    {
        QList<UserInfo>& currentMemberList = (*this->memberList)[chatSessionId];
        currentMemberList.clear();

        for (const auto& m : memberList)
        {
            UserInfo userInfo;
            userInfo.Load(m);
            currentMemberList.push_back(userInfo);
        }
    }

    void DataCenter::SearchUserAsync(const QString &searchKey)
    {
        netClient.SearchUser(loginSessionId, searchKey);
    }

    void DataCenter::ResetSearchUserResult(const QList<SnowK::UserInfo> &userList)
    {
        if (searchUserResult == nullptr)
        {
            searchUserResult = new QList<UserInfo>();
        }
        searchUserResult->clear();

        for (const auto& u : userList)
        {
            UserInfo userInfo;
            userInfo.Load(u);
            searchUserResult->push_back(userInfo);
        }
    }

    void DataCenter::SearchMessageAsync(const QString &searchKey)
    {
        netClient.SearchMessage(loginSessionId, this->currentChatSessionId, searchKey);
    }

    void DataCenter::SearchMessageByTimeAsync(const QDateTime &begTime, const QDateTime &endTime)
    {
        netClient.SearchMessageByTime(loginSessionId, currentChatSessionId, begTime, endTime);
    }

    void DataCenter::ResetSearchMessageResult(const QList<SnowK::MessageInfo> &msgList)
    {
        if (this->searchMessageResult == nullptr)
        {
            this->searchMessageResult = new QList<Message>();
        }
        this->searchMessageResult->clear();

        for (const auto& m : msgList)
        {
            Message message;
            message.Load(m);
            searchMessageResult->push_back(message);
        }
    }

    void DataCenter::UserLoginAsync(const QString &username, const QString &password)
    {
        // Login operation, no loginSessionId.
        // After successful login, the server will return loginSessionId
        netClient.UserLogin(username, password);
    }

    void DataCenter::ResetLoginSessionId(const QString &loginSessionId)
    {
        this->loginSessionId = loginSessionId;

        // Once the session id is changed, it needs to be saved to the hard disk.
        SaveDataFile();
    }

    void DataCenter::UserRegisterAsync(const QString &username, const QString &password)
    {
        netClient.UserRegister(username, password);
    }

    void DataCenter::PhoneLoginAsync(const QString &phone, const QString &verifyCode)
    {
        netClient.PhoneLogin(phone, this->currentVerifyCodeId, verifyCode);
    }

    void DataCenter::PhoneRegisterAsync(const QString &phone, const QString &verifyCode)
    {
        netClient.PhoneRegister(phone, this->currentVerifyCodeId, verifyCode);
    }

    void DataCenter::GetSingleFileAsync(const QString &fileId)
    {
        netClient.GetSingleFile(loginSessionId, fileId);
    }

    //////////////////////////////////////////////////////////////////
    /// Helper functions
    //////////////////////////////////////////////////////////////////

    ChatSessionInfo *DataCenter::FindChatSessionById(const QString &chatSessionId)
    {
        if (chatSessionList == nullptr)
        {
            return nullptr;
        }

        for (auto& info : *chatSessionList)
        {
            if (info.chatSessionId == chatSessionId)
            {
                return &info;
            }
        }

        return nullptr;
    }

    ChatSessionInfo *DataCenter::FindChatSessionByUserId(const QString &userId)
    {
        if (chatSessionList == nullptr)
        {
            return nullptr;
        }

        for (auto& info : *chatSessionList)
        {
            if (info.userId == userId)
            {
                return &info;
            }
        }

        return nullptr;
    }

    UserInfo *DataCenter::FindFriendById(const QString &userId)
    {
        if (this->friendList == nullptr)
        {
            return nullptr;
        }

        for (auto& f : *friendList)
        {
            if (f.userId == userId)
            {
                return &f;
            }
        }

        return nullptr;
    }

    void DataCenter::TopChatSessionInfo(const ChatSessionInfo &chatSessionInfo)
    {
        if (chatSessionList == nullptr)
        {
            return;
        }

        // Find
        auto it = chatSessionList->begin();
        for (; it != chatSessionList->end(); ++it)
        {
            if (it->chatSessionId == chatSessionInfo.chatSessionId)
            {
                break;
            }
        }

        if (it == chatSessionList->end())
        {
            return;
        }

        // Pin
        ChatSessionInfo backup = chatSessionInfo;
        chatSessionList->erase(it);
        chatSessionList->push_front(backup);
    }

    void DataCenter::SetCurrentChatSessionId(const QString &chatSessionId)
    {
        this->currentChatSessionId = chatSessionId;
    }

    const QString &DataCenter::GetCurrentChatSessionId()
    {
        return this->currentChatSessionId;
    }

    void DataCenter::AddMessage(const Message &message)
    {
        QList<Message>& messageList = (*recentMessages)[message.chatSessionId];
        messageList.push_back(message);
    }

    void DataCenter::ClearUnread(const QString &chatSessionId)
    {
        (*unreadMessageCount)[chatSessionId] = 0;
        SaveDataFile();
    }

    void DataCenter::AddUnread(const QString &chatSessionId)
    {
        ++(*unreadMessageCount)[chatSessionId];
        SaveDataFile();
    }

    int DataCenter::GetUnread(const QString &chatSessionId)
    {
        return (*unreadMessageCount)[chatSessionId];
    }
}
