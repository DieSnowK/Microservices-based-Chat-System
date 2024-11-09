#include "sessionfriendarea.h"
#include "mainwidget.h"
#include "model/data.hpp"
#include "debug.hpp"
#include "model/datacenter.h"

using model::DataCenter;

//////////////////////////////////////////////////////////
/// SessionFriendArea implementation
//////////////////////////////////////////////////////////

SessionFriendArea::SessionFriendArea(QWidget *parent)
    : QScrollArea{parent}
{
    // This property is set to enable the scrolling effect
    this->setWidgetResizable(true);
    this->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 2px; background-color: rgb(46, 46, 46); }");
    this->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal { height: 0px; }");
    this->setStyleSheet("QWidget { border: none; }");

    container = new QWidget();
    container->setFixedWidth(310);
    this->setWidget(container);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop);
    container->setLayout(layout);

#if TEST_UI
    for(int i = 0; i < 30; ++i)
    {
        QIcon icon(":/resource/image/defaultAvatar.png");
        this->AddItem(ItemType::APPLY_ITEM_TYPE, QString::number(i), icon,
                      "SnowK " + QString::number(i), "Last Msg " + QString::number(i));
    }
#endif
}

void SessionFriendArea::Clear()
{
    QLayout* layout = container->layout();
    for(int i = layout->count() - 1; i >= 0; --i)
    {
        QLayoutItem* item = layout->takeAt(i);
        if(item->widget())
        {
            delete item->widget();
        }
    }
}

void SessionFriendArea::AddItem(ItemType itemtype, const QString& id, const QIcon &avatar,
                                const QString &name, const QString &text)
{
    SessionFriendItem* item = nullptr;
    switch(itemtype)
    {
    case ItemType::SESSION_ITEM_TYPE:
        item = new SessionItem(this, id, avatar, name, text);
        break;
    case ItemType::FRIEND_ITEM_TYPE:
        item = new FriendItem(this, id, avatar, name, text);
        break;
    case ItemType::APPLY_ITEM_TYPE:
        item = new ApplyItem(this, id, avatar, name);
        break;
    default:
        LOG() << "Error ItemType, itemtype = " << (int)itemtype;
        break;
    }
    container->layout()->addWidget(item);
}

void SessionFriendArea::ClickItem(int index)
{
    if (index < 0 || index >= container->layout()->count())
    {
        LOG() << "Click the element's subscript out of range, index = " << index;
        return;
    }

    QLayoutItem* layoutItem = container->layout()->itemAt(index);
    if (layoutItem == nullptr || layoutItem->widget() == nullptr)
    {
        LOG() << "The specified element does not exist, index = " << index;
        return;
    }

    SessionFriendItem* item = dynamic_cast<SessionFriendItem*>(layoutItem->widget());
    item->Select();
}

//////////////////////////////////////////////////////////
/// SessionFriendItem implementation
//////////////////////////////////////////////////////////

SessionFriendItem::SessionFriendItem(QWidget *owner, const QIcon &avatar,
                                     const QString &name, const QString &text)
    : owner(owner)
{
    this->setFixedHeight(70);
    this->setStyleSheet("QWidget { background-color: rgb(231, 231, 231)} ");

    QGridLayout* layout = new QGridLayout();
    layout->setContentsMargins(20, 0, 0, 0);
    layout->setHorizontalSpacing(10);
    layout->setVerticalSpacing(0);
    this->setLayout(layout);

    QPushButton* avatarBtn = new QPushButton();
    avatarBtn->setFixedSize(50, 50);
    avatarBtn->setIconSize(QSize(50, 50));
    avatarBtn->setIcon(avatar);
    avatarBtn->setStyleSheet("QPushButton { border: none; }");
    avatarBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel* nameLabel = new QLabel();
    nameLabel->setText(name);
    nameLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: 600; }");
    nameLabel->setFixedHeight(35);
    nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    messageLabel = new QLabel();
    messageLabel->setText(text);
    messageLabel->setFixedHeight(35);
    messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout->addWidget(avatarBtn, 0, 0, 2, 2);
    layout->addWidget(nameLabel, 0, 2, 1, 8);
    layout->addWidget(messageLabel, 1, 2, 1, 8);
}

void SessionFriendItem::paintEvent(QPaintEvent *event)
{
    (void)event;

    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SessionFriendItem::mousePressEvent(QMouseEvent *event)
{
    (void)event;
    Select();
}

void SessionFriendItem::enterEvent(QEnterEvent *event)
{
    (void)event;

    if(this->selected)
    {
        return;
    }
    this->setStyleSheet("QWidget { background-color: rgb(215, 215, 215); }");
}

void SessionFriendItem::leaveEvent(QEvent *event)
{
    (void)event;

    if(this->selected)
    {
        return;
    }
    this->setStyleSheet("QWidget { background-color: rgb(231, 231, 231); }");
}

void SessionFriendItem::Select()
{
    const QObjectList children = this->parentWidget()->children();
    for(QObject* child : children)
    {
        if(!child->isWidgetType())
        {
            continue;
        }

        SessionFriendItem* item = dynamic_cast<SessionFriendItem*>(child);
        {
            if(item->selected)
            {
                item->selected = false;
                item->setStyleSheet("QWidget { background-color: rgb(231, 231, 231); }");
            }
        }
    }

    this->selected = true;
    this->setStyleSheet("QWidget { background-color: rgb(210, 210, 210); }");

    this->Active();
}

void SessionFriendItem::Active()
{}

//////////////////////////////////////////////////////////
/// SessionItem
//////////////////////////////////////////////////////////

SessionItem::SessionItem(QWidget* owner, const QString& chatSessionId,
                         const QIcon& avatar, const QString& name, const QString& lastMessage)
    : SessionFriendItem(owner, avatar, name, lastMessage)
    , chatSessionId(chatSessionId)
    , text(lastMessage)
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    connect(dataCenter, &DataCenter::UpdateLastMessage, this, &SessionItem::UpdateLastMessage);

    // The number of unread messages needs to be displayed in order to support the
        // correct display of unread messages even after the client restarts.
    int unread = dataCenter->GetUnread(chatSessionId);
    if (unread > 0)
    {
        this->messageLabel->setText(QString("[Unread %1 entry] ").arg(unread) + text);
    }
}

void SessionItem::Active()
{
    LOG() << "Active SessionItem, chatSessionId = " << chatSessionId;

    MainWidget* mainWidget = MainWidget::GetInstance();
    mainWidget->LoadRecentMessage(chatSessionId);

    // Clears the data for unread messages and updates the display
    DataCenter* dataCenter = DataCenter::GetInstance();
    dataCenter->ClearUnread(chatSessionId);

    // Updated the display of the interface.
    // Preview the conversation message here, and get
        //rid of the "[unread x]" content in front of it
    this->messageLabel->setText(text);
}

void SessionItem::UpdateLastMessage(const QString &chatSessionId)
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    if (this->chatSessionId != chatSessionId)
    {
        return;
    }

    // Get the last message
    QList<Message>* messageList = dataCenter->GetRecentMessageList(chatSessionId);
    if (messageList == nullptr || messageList->size() == 0)
    {
        return;
    }
    const Message& lastMessage = messageList->back();

    // Clearly displayed text content
    if (lastMessage.msgType == MessageType::TEXT_TYPE)
    {
        text = lastMessage.content;
    }
    else if (lastMessage.msgType == MessageType::IMAGE_TYPE)
    {
        text = "[Image]";
    }
    else if (lastMessage.msgType == MessageType::FILE_TYPE)
    {
        text = "[File]";
    }
    else if (lastMessage.msgType == MessageType::SPEECH_TYPE)
    {
        text = "[Speech]";
    }
    else
    {
        LOG() << "Error MessageType";
        return;
    }

    // The preview content is displayed on the interface
    if (chatSessionId == dataCenter->GetCurrentChatSessionId())
    {
        this->messageLabel->setText(text);
    }
    else
    {
        int unread = dataCenter->GetUnread(chatSessionId);
        if (unread > 0)
        {
            this->messageLabel->setText(QString("[Unread %1 entry] ").arg(unread) + text);
        }
    }
}

//////////////////////////////////////////////////////////
/// FriendItem
//////////////////////////////////////////////////////////

FriendItem::FriendItem(QWidget* owner, const QString& userId, const QIcon& avatar,
                       const QString& name, const QString& description)
    : SessionFriendItem(owner, avatar, name, description)
    , userId(userId)
{
    
}

void FriendItem::Active()
{
    LOG() << "Active FriendItem, chatSessionId = " << userId;

    MainWidget* mainWidget = MainWidget::GetInstance();
    mainWidget->SwitchToSession(userId);
}

//////////////////////////////////////////////////////////
/// ApplyItem
//////////////////////////////////////////////////////////

ApplyItem::ApplyItem(QWidget* owner, const QString& userId,
                     const QIcon& avatar, const QString& name)
    : SessionFriendItem(owner, avatar, name, "")
    , userId(userId)
{
    QGridLayout* layout = dynamic_cast<QGridLayout*>(this->layout());
    layout->removeWidget(messageLabel);
    delete messageLabel;

    QPushButton* acceptBtn = new QPushButton();
    acceptBtn->setText("Accept");
    QPushButton* rejectBtn = new QPushButton();
    rejectBtn->setText("Reject");

    layout->addWidget(acceptBtn, 1, 2, 1, 1);
    layout->addWidget(rejectBtn, 1, 3, 1, 1);
}

void ApplyItem::Active()
{
    LOG() << "Active ApplyItem, chatSessionId = " << userId;

}
