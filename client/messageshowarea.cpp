#include "messageshowarea.h"

MessageShowArea::MessageShowArea() 
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setWidgetResizable(true);
    this->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 2px; \
                                              background-color: rgb(240, 240, 240); }");
    this->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal { height: 0; }");
    this->setStyleSheet("QScrollArea { border: none; }");

    container = new QWidget();
    this->setWidget(container);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    container->setLayout(layout);

#if TEST_UI
    UserInfo userInfo;
    userInfo.userId = QString::number(1000);
    userInfo.nickname = "SnowK";
    userInfo.description = "Cool Boy~";
    userInfo.avatar = QIcon(":/resource/image/defaultAvatar.png");
    userInfo.phone = "18351958129";
    Message message = Message::MakeMessage(TEXT_TYPE, "", userInfo, QString("This is a test message").toUtf8(), "");
    this->addMessage(false, message);

    for (int i = 1; i <= 30; ++i)
    {
        UserInfo userInfo;
        userInfo.userId = QString::number(1000 + i);
        userInfo.nickname = "SnowK" + QString::number(i);
        userInfo.description = "Cool Boy~";
        userInfo.avatar = QIcon(":/resource/image/defaultAvatar.png");
        userInfo.phone = "18351958129";
        Message message = Message::MakeMessage(TEXT_TYPE, "", userInfo, (QString("This is a test message") + QString::number(i)).toUtf8(), "");
        this->addMessage(true, message);
    }
#endif
}

MessageItem::MessageItem(bool isLeft) 
    : isLeft(isLeft)
{}

MessageItem *MessageItem::MakeMessageItem(bool isLeft, const Message &message)
{
    MessageItem* messageItem = new MessageItem(isLeft);

    QGridLayout* layout = new QGridLayout();
    layout->setContentsMargins(30, 10, 40, 0);
    layout->setSpacing(10);
    messageItem->setMinimumHeight(100);
    messageItem->setLayout(layout);

    QPushButton* avatarBtn = new QPushButton();
    avatarBtn->setFixedSize(40, 40);
    avatarBtn->setIconSize(QSize(40, 40));
    avatarBtn->setIcon(message.sender.avatar);
    avatarBtn->setStyleSheet("QPushButton { border: none; }");
    if (isLeft)
    {
        layout->addWidget(avatarBtn, 0, 0, 2, 1, Qt::AlignTop | Qt::AlignLeft);
    }
    else
    {
        layout->addWidget(avatarBtn, 0, 1, 2, 1, Qt::AlignTop | Qt::AlignLeft);
    }

    QLabel* nameLabel = new QLabel();
    nameLabel->setText(message.sender.nickname + " | " + message.time);
    nameLabel->setAlignment(Qt::AlignBottom);
    nameLabel->setStyleSheet("QLabel { font-size: 12px; color: rgb(178, 178, 178); }");
    if (isLeft)
    {
        layout->addWidget(nameLabel, 0, 1);
    }
    else
    {
        layout->addWidget(nameLabel, 0, 0, Qt::AlignRight);
    }

    QWidget* contentWidget = nullptr;
    switch (message.messageType)
    {
    case TEXT_TYPE:
        contentWidget = MakeTextMessageItem(isLeft, message.content);
        break;
    case IMAGE_TYPE:
        contentWidget = MakeImageMessageItem(isLeft, message.fileId, message.content);
        break;
    case FILE_TYPE:
        contentWidget = MakeFileMessageItem(isLeft, message);
        break;
    case SPEECH_TYPE:
        contentWidget = MakeSpeechMessageItem(isLeft, message);
        break;
    default:
        LOG() << "Error MessageType, messageType = " << (int)message.messageType;
    }
    if (isLeft)
    {
        layout->addWidget(contentWidget, 1, 1);
    }
    else
    {
        layout->addWidget(contentWidget, 1, 0);
    }

    return messageItem;
}
