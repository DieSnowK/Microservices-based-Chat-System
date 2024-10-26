#include "messageshowarea.h"
#include "debug.hpp"

////////////////////////////////////////////////////////
/// MessageShowArea
////////////////////////////////////////////////////////

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
    Message message = Message::MakeMessage(MessageType::TEXT_TYPE, "", userInfo,
                        QString("This is a test message This is a test message This is a test message This is a test message This is \
                                a test message This is a test message This is a test message This is a test message").toUtf8(), "");
    this->AddMessage(false, message);

    for (int i = 1; i <= 30; ++i)
    {
        UserInfo userInfo;
        userInfo.userId = QString::number(1000 + i);
        userInfo.nickname = "SnowK" + QString::number(i);
        userInfo.description = "Cool Boy~";
        userInfo.avatar = QIcon(":/resource/image/defaultAvatar.png");
        userInfo.phone = "18351958129";
        Message message = Message::MakeMessage(MessageType::TEXT_TYPE, "", userInfo,
                            (QString("This is a test message") + QString::number(i)).toUtf8(), "");
        this->AddMessage(true, message);
    }
#endif
}

void MessageShowArea::AddMessage(bool isLeft, const Message &message)
{
    MessageItem* msgItem = MessageItem::MakeMessageItem(isLeft, message);
    container->layout()->addWidget(msgItem);
}

void MessageShowArea::AddFrontMessage(bool isLeft, const Message &message)
{
    MessageItem* msgItem = MessageItem::MakeMessageItem(isLeft, message);
    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(container->layout());
    layout->insertWidget(0, msgItem);
}

void MessageShowArea::Clear()
{
    QLayout* layout = container->layout();
    for (int i = layout->count() - 1; i >= 0; --i)
    {
        QLayoutItem* item = layout->takeAt(i);
        if (item && item->widget())
        {
            delete item->widget();
        }
    }
}

////////////////////////////////////////////////////////
/// MessageItem
////////////////////////////////////////////////////////

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
    switch (message.msgType)
    {
    case MessageType::TEXT_TYPE:
        contentWidget = MakeTextMessageItem(isLeft, message.content);
        break;
    case MessageType::IMAGE_TYPE:
        contentWidget = MakeImageMessageItem(isLeft, message.fileId, message.content);
        break;
    case MessageType::FILE_TYPE:
        contentWidget = MakeFileMessageItem(isLeft, message);
        break;
    case MessageType::SPEECH_TYPE:
        contentWidget = MakeSpeechMessageItem(isLeft, message);
        break;
    default:
        LOG() << "Error MessageType, messageType = " << (int)message.msgType;
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

QWidget *MessageItem::MakeTextMessageItem(bool isLeft, const QString &text)
{
    MessageContentLabel* msgContentLabel = new MessageContentLabel(text, isLeft, MessageType::TEXT_TYPE, "", QByteArray());
    return msgContentLabel;
}

QWidget *MessageItem::MakeImageMessageItem(bool isLeft, const QString &fileId, const QByteArray &content)
{
    return nullptr;
}

QWidget *MessageItem::MakeFileMessageItem(bool isLeft, const Message &message)
{
    return nullptr;
}

QWidget *MessageItem::MakeSpeechMessageItem(bool isLeft, const Message &message)
{
    return nullptr;
}

////////////////////////////////////////////////////////
/// MessageContentLabel
////////////////////////////////////////////////////////

MessageContentLabel::MessageContentLabel(const QString &text, bool isLeft, MessageType msgType,
                                         const QString &fileId, const QByteArray &content)
    : isLeft(isLeft)
    , msgType(msgType)
    , fileId(fileId)
    , content(content)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QFont font;
    font.setFamily("微软雅黑");
    font.setPixelSize(16);

    this->label = new QLabel(this);
    this->label->setText(text);
    this->label->setFont(font);
    this->label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    this->label->setWordWrap(true);
    this->label->setStyleSheet("QLabel { padding: 0 10px; line-height: 1.2; background-color: transparent; }");

    // if (msgType == MessageType::TEXT_TYPE)
    // {
    //     return;
    // }

    // if (this->content.isEmpty())
    // {
    //     DataCenter* dataCenter = DataCenter::getInstance();
    //     connect(dataCenter, &DataCenter::getSingleFileDone, this, &MessageContentLabel::updateUI);
    //     dataCenter->getSingleFileAsync(this->fileId);
    // }
    // else
    // {
    //     // content 不为空, 说明当前的这个数据就是已经现成. 直接就把 表示加载状态的变量设为 true
    //     this->loadContentDone = true;
    // }
}

// TODO
void MessageContentLabel::paintEvent(QPaintEvent *event)
{
    (void) event;

    // 1.Gets the width of the parent element
    QObject* object = this->parent();
    if (!object->isWidgetType())
    {
        return;
    }
    QWidget* parent = dynamic_cast<QWidget*>(object);
    int width = parent->width() * 0.6;

    // 2. Calculate how wide the current text needs to be if it is placed on a single line
    QFontMetrics metrics(this->label->font());
    int totalWidth = metrics.horizontalAdvance(this->label->text());

    // 3. Calculate what the number of rows is here
        // 40 means there are 20px margins on the left and right
    int rows = (totalWidth / (width - 40)) + 1;
    if (rows == 1)
    {
        width = totalWidth + 40;
    }

    // 4. Based on the number of rows, the height is calculated
        // 20 means there are 10px margins on top and bottom
    int height = rows * (this->label->font().pixelSize() * 1.2 ) + 20;

    // 5. Draw rounded rectangles and arrows
    QPainter painter(this);
    QPainterPath path;
    painter.setRenderHint(QPainter::Antialiasing);

    if (isLeft)
    {
        painter.setPen(QPen(QColor(255, 255, 255)));
        painter.setBrush(QColor(255, 255, 255));

        painter.drawRoundedRect(10, 0, width, height, 10, 10);

        path.moveTo(10, 15);
        path.lineTo(0, 20);
        path.lineTo(10, 25);

        // Drawn lines form closed polygons before they
            // can be filled with color using a brush
        path.closeSubpath();
        painter.drawPath(path); // Real drawing operations

        this->label->setGeometry(10, 0, width, height);
    }
    else
    {
        painter.setPen(QPen(QColor(137, 217, 97)));
        painter.setBrush(QColor(137, 217, 97));

        // 10 is the width used to accommodate the arrow
        int leftPos = this->width() - width - 10;
        int rightPos = this->width() - 10;

        painter.drawRoundedRect(leftPos, 0, width, height, 10, 10);

        path.moveTo(rightPos, 15);
        path.lineTo(rightPos + 10, 20);
        path.lineTo(rightPos, 25);
        path.closeSubpath();
        painter.drawPath(path);

        this->label->setGeometry(leftPos, 0, width, height);
    }

    // 6.Re-set the height of the parent element to make sure that
        // the parent element is tall enough to accommodate the area
        // where the message is displayed as drawn above
    // Attention: The height should cover the height of the label before
        // the name and time, as well as leave some space for redundancy
    parent->setFixedHeight(height + 50);
}
