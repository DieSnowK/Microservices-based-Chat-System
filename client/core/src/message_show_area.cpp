#include <QTimer>
#include <QPainterPath>
#include <QPainter>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMenu>

#include "message_show_area.h"
#include "debug.h"
#include "mainwidget.h"
#include "datacenter.h"
#include "data.hpp"
#include "sound_recorder.h"
#include "userinfo_widget.h"
#include "toast.h"

using model::DataCenter;
using model::MessageType;

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

void MessageShowArea::ScrollToEnd()
{
    // In order to make the scrolling effect better, set the scrollbar after the interface is drawn
        // Add a "delay" to the scrolling operation here
    QTimer* timer = new QTimer();
    connect(timer, &QTimer::timeout, this, [=]()
    {
        int maxValue = this->verticalScrollBar()->maximum();
        this->verticalScrollBar()->setValue(maxValue);

        timer->stop();
        timer->deleteLater();
    });
    timer->start(500);
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

    connect(avatarBtn, &QPushButton::clicked, messageItem, [=]()
    {
        MainWidget* mainWidget = MainWidget::GetInstance();
        UserInfoWidget* userInfoWidget = new UserInfoWidget(message.sender, mainWidget);
        userInfoWidget->exec();
    });

    if (!isLeft)
    {
        DataCenter* dataCenter = DataCenter::GetInstance();
        connect(dataCenter, &DataCenter::ChangeNicknameDone, messageItem, [=]()
        {
            nameLabel->setText(dataCenter->GetMyself()->nickname + " | " + message.time);
        });

        connect(dataCenter, &DataCenter::ChangeAvatarDone, messageItem, [=]()
        {
            UserInfo* myself = dataCenter->GetMyself();
            avatarBtn->setIcon(myself->avatar);
        });
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
    MessageImageLabel* msgImageLabel = new MessageImageLabel(fileId, content, isLeft);
    return msgImageLabel;
}

QWidget *MessageItem::MakeFileMessageItem(bool isLeft, const Message &message)
{
    MessageContentLabel* messageContentLabel = new MessageContentLabel("[File] " + message.fileName, isLeft, message.msgType,
                                                                       message.fileId, message.content);
    return messageContentLabel;
}

QWidget *MessageItem::MakeSpeechMessageItem(bool isLeft, const Message &message)
{
    MessageContentLabel* messageContentLabel = new MessageContentLabel("[Speech]", isLeft, message.msgType,
                                                                       message.fileId, message.content);
    return messageContentLabel;
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
    font.setFamily("寰蒋闆呴粦");
    font.setPixelSize(16);

    this->label = new QLabel(this);
    this->label->setText(text);
    this->label->setFont(font);
    this->label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    this->label->setWordWrap(true);
    this->label->setStyleSheet("QLabel { padding: 0 10px; line-height: 1.2; background-color: transparent; }");

    if (msgType == MessageType::TEXT_TYPE)
    {
        return;
    }

    // For file messages, and when content is empty, load data through the network
    if (this->content.isEmpty())
    {
        DataCenter* dataCenter = DataCenter::GetInstance();
        connect(dataCenter, &DataCenter::GetSingleFileDone, this, &MessageContentLabel::UpdateUI);
        dataCenter->GetSingleFileAsync(this->fileId);
    }
    else
    {
        this->loadContentDone = true;
    }
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

void MessageContentLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (this->msgType == MessageType::FILE_TYPE)
        {
            if (!this->loadContentDone)
            {
                Toast::ShowMessage("The data has not been loaded successfully, pls try again later.");
                return;
            }

            SaveAsFile(this->content);
        }
        else if (this->msgType == MessageType::SPEECH_TYPE)
        {
            if (!this->loadContentDone)
            {
                Toast::ShowMessage("The data has not been loaded successfully, pls try again later.");
                return;
            }

            SoundRecorder* soundRecorder = SoundRecorder::GetInstance();
            this->label->setText("Playing...");
            connect(soundRecorder, &SoundRecorder::SoundPlayDone, this, &MessageContentLabel::PlayDone, Qt::UniqueConnection);
            soundRecorder->StartPlay(this->content);
        }
        else
        {}
    }
}

void MessageContentLabel::UpdateUI(const QString &fileId, const QByteArray &fileContent)
{
    if (fileId != this->fileId)
    {
        return;
    }

    this->content = fileContent;
    this->loadContentDone = true;

    this->update(); // It鈥檚 okay if you don鈥檛 have it
}

void MessageContentLabel::SaveAsFile(const QByteArray &content)
{
    QString filePath = QFileDialog::getSaveFileName(this, "Save as", QDir::homePath(), "*");
    if (filePath.isEmpty())
    {
        LOG() << "User canceled file save as";
        return;
    }

    model::Util::WriteByteArrayToFile(filePath, content);
}

void MessageContentLabel::PlayDone()
{
    if (this->label->text() == "Playing...")
    {
        this->label->setText("[Speech]");
    }
}

void MessageContentLabel::contextMenuEvent(QContextMenuEvent *event)
{
    (void) event;
    if (msgType != model::MessageType::SPEECH_TYPE)
    {
        LOG() << "Non-voice messages do not currently support right-click menus";
        return;
    }

    QMenu* menu = new QMenu(this);
    QAction* action = menu->addAction("Speech to text");
    menu->setStyleSheet("QMenu { color: rgb(0, 0, 0); }");

    connect(action, &QAction::triggered, this, [=]()
    {
        DataCenter* dataCenter = DataCenter::GetInstance();
        connect(dataCenter, &DataCenter::SpeechConvertTextDone, this,
                &MessageContentLabel::SpeechConvertTextDone, Qt::UniqueConnection);
        dataCenter->SpeechConvertTextAsync(this->fileId, this->content);
    });

    // A "modal dialog" pops up here to display the menu/menu items
    menu->exec(event->globalPos());

    delete menu;
}

void MessageContentLabel::SpeechConvertTextDone(const QString &fileId, const QString &text)
{
    if (this->fileId != fileId)
    {
        return;
    }

    this->label->setText("[Speech to text] " + text);
    this->update();
}

////////////////////////////////////////////////////////
/// MessageImageLabel
////////////////////////////////////////////////////////

// For this class, there are the following two situations
    // 1.fileid = "", content has content
        // Sending a message locally to the server
    // 2.filed has content, content = ""
        // Received a message pushed by the server, containing only fileId
        // If you want to get the real data of the image, you need to send a
            // request to the server again to get the file content based on fileId.
MessageImageLabel::MessageImageLabel(const QString &fileId, const QByteArray &content, bool isLeft)
    : fileId(fileId), content(content), isLeft(isLeft)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    imageBtn = new QPushButton(this);
    imageBtn->setStyleSheet("QPushButton { border: none; }");

    if (content.isEmpty())
    {
        DataCenter* dataCenter = DataCenter::GetInstance();
        connect(dataCenter, &DataCenter::GetSingleFileDone, this, &MessageImageLabel::UpdateUI);
        dataCenter->GetSingleFileAsync(fileId);
    }
}

void MessageImageLabel::UpdateUI(const QString &fileId, const QByteArray &content)
{
    if (this->fileId != fileId)
    {
        return;
    }

    this->content = content;
    this->update(); // In order to call paintEvent()
}

void MessageImageLabel::paintEvent(QPaintEvent *event)
{
    (void) event;

    // 1.The width of the image shown here is 60% of the width of the top parent element
    QObject* object = this->parent();
    if (!object->isWidgetType())
    {
        return;
    }
    QWidget* parent = dynamic_cast<QWidget*>(object);
    int width = parent->width() * 0.6;

    // 2.Load binary data as image object
    QImage image;
    if (content.isEmpty())
    {
        // At this time, the response data of the image has not been returned yet.
            // Here, let鈥檚 replace it with a 鈥渇ixed default image鈥?.
        QByteArray tmpContent = model::Util::LoadFileToByteArray(":/resource/image/image.png");
        image.loadFromData(tmpContent);
    }
    else
    {
        image.loadFromData(content); // Automatic recognition
    }

    // 3.Zoom to image.
    int height = 0;
    if (image.width() > width)
    {
        // Proportional scaling.
        height = ((double)image.height() / image.width()) * width;
    }
    else
    {
        width = image.width();
        height = image.height();
    }

    // QImage cannot be directly converted into QIcon. QPixmap is required to convert it.
    QPixmap pixmap = QPixmap::fromImage(image);
    imageBtn->setIconSize(QSize(width, height));
    imageBtn->setIcon(QIcon(pixmap));

    // 4.Since the height of the image is calculated, the height of the
        // parent object of the element can accommodate the current element.
    // The + 50 here is to accommodate the "name" part above.
        // At the same time, it leaves a little redundant space.
    parent->setFixedHeight(height + 50);

    // 5.Determine where the button is.
    if (isLeft)
    {
        imageBtn->setGeometry(10, 0, width, height);
    }
    else
    {
        int leftPos = this->width() - width - 10;
        imageBtn->setGeometry(leftPos, 0, width, height);
    }
}
