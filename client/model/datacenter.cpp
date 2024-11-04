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
}
