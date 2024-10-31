#include "datacenter.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>

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
        for(auto it = unreadMessageCount->begin(); it != unreadMessageCount.end(); ++it)
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
            LOG << "Failed to parse ChatClient.json";
            file.close();
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        this->loginSessionId = jsonObj["loginSessionId"].toString();

        this->unreadMessageCount->clear();
        QJsonObject jsonUnread = jsonObj["unread"].toObject();
        for(auto it = unreadMessageCount->begin(); it != unreadMessageCount.end(); ++it)
        {
            this->unreadMessageCount->insert(it.key(), it.value().toInt());
        }

        file.close();
    }
}
