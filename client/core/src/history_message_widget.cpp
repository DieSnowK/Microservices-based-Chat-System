#include <QFileDialog>
#include <QScrollArea>
#include <QScrollBar>

#include "history_message_widget.h"
#include "debug.h"
#include "datacenter.h"
#include "toast.h"
#include "sound_recorder.h"

using model::DataCenter;

////////////////////////////////////////////////////////////////////
/// HistoryItem
////////////////////////////////////////////////////////////////////

HistoryItem *HistoryItem::MakeHistoryItem(const Message &message)
{
    HistoryItem* item = new HistoryItem();
    item->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QGridLayout* layout = new QGridLayout();
    layout->setVerticalSpacing(0);
    layout->setHorizontalSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);
    item->setLayout(layout);

    QPushButton* avatarBtn = new QPushButton();
    avatarBtn->setFixedSize(40, 40);
    avatarBtn->setIconSize(QSize(40, 40));
    avatarBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    avatarBtn->setIcon(message.sender.avatar);
    avatarBtn->setStyleSheet("QPushButton { border: none; }");

    QLabel* nameLabel = new QLabel();
    nameLabel->setText(message.sender.nickname + " | " + message.time);
    nameLabel->setFixedHeight(20);  // The height is set to half the height of the avatar
    nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QWidget* contentWidget = nullptr;
    if (message.msgType == MessageType::TEXT_TYPE)
    {
        QLabel* label = new QLabel();
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        label->setWordWrap(true);
        label->setText(QString(message.content));
        label->adjustSize();    // Settings allow labels to resize automatically
        contentWidget = label;
    }
    else if (message.msgType == MessageType::IMAGE_TYPE)
    {
        contentWidget = new ImageButton(message.fileId, message.content);
    }
    else if (message.msgType == MessageType::FILE_TYPE)
    {
        contentWidget = new FileLabel(message.fileId, message.fileName);
    }
    else if (message.msgType == MessageType::SPEECH_TYPE)
    {
        contentWidget = new SpeechLabel(message.fileId);
    }
    else
    {
        LOG() << "Error Message Type, messageType = " << (int)message.msgType;
    }

    layout->addWidget(avatarBtn, 0, 0, 2, 1);
    layout->addWidget(nameLabel, 0, 1, 1, 1);
    layout->addWidget(contentWidget, 1, 1, 5, 1);

    return item;
}

////////////////////////////////////////////////////////////////////
/// HistoryMessageWidget
////////////////////////////////////////////////////////////////////

HistoryMessageWidget::HistoryMessageWidget(QWidget* parent)
    : QDialog(parent)
{
    this->setFixedSize(600, 600);
    this->setWindowTitle("History Messages");
    this->setWindowIcon(QIcon(":/resource/image/logo.png"));
    this->setStyleSheet("QWidget { background-color: rgb(255, 255, 255); }");
    this->setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(10);
    layout->setContentsMargins(30, 30, 30, 0);
    this->setLayout(layout);

    keyRadioBtn = new QRadioButton();
    timeRadioBtn = new QRadioButton();
    keyRadioBtn->setText("Search by keyword");
    timeRadioBtn->setText("Search by time");
    keyRadioBtn->setChecked(true); // By default, search by keyword
    layout->addWidget(keyRadioBtn, 0, 0, 1, 2);
    layout->addWidget(timeRadioBtn, 0, 2, 1, 2);

    searchEdit = new QLineEdit();
    searchEdit->setFixedHeight(50);
    searchEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    searchEdit->setPlaceholderText("Keyword");
    searchEdit->setStyleSheet("QLineEdit { border: none; border-radius: 10px; "
                              "background-color: rgb(240, 240, 240); "
                              "font-size: 16px; padding-left: 10px; }");
    layout->addWidget(searchEdit, 1, 0, 1, 8);

    QPushButton* searchBtn = new QPushButton();
    searchBtn->setFixedSize(50, 50);
    searchBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    searchBtn->setIconSize(QSize(30, 30));
    searchBtn->setIcon(QIcon(":/resource/image/search.png"));
    QString btnStyle = "QPushButton { border: none; border-radius: 10px; "
                       "background-color: rgb(240, 240, 240); }";
    btnStyle += "QPushButton:pressed { background-color: rgb(220, 220, 220); }";
    searchBtn->setStyleSheet(btnStyle);
    layout->addWidget(searchBtn, 1, 8, 1, 1);

    // Initially, it is to be hidden
    QLabel* begTag = new QLabel();
    begTag->setText("Start time");
    QLabel* endTag = new QLabel();
    endTag->setText("End time");
    begTimeEdit = new QDateTimeEdit();
    endTimeEdit = new QDateTimeEdit();
    begTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
    endTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
    begTimeEdit->setFixedHeight(40);
    endTimeEdit->setFixedHeight(40);
    begTag->hide();
    endTag->hide();
    begTimeEdit->hide();
    endTimeEdit->hide();

    InitScrollArea(layout);

    connect(keyRadioBtn, &QRadioButton::clicked, this, [=]()
    {
        layout->removeWidget(begTag);
        layout->removeWidget(begTimeEdit);
        layout->removeWidget(endTag);
        layout->removeWidget(endTimeEdit);
        begTag->hide();
        begTimeEdit->hide();
        endTag->hide();
        endTimeEdit->hide();

        layout->addWidget(searchEdit, 1, 0, 1, 8);
        searchEdit->show();
    });

    connect(timeRadioBtn, &QRadioButton::clicked, this, [=]()
    {
        layout->removeWidget(searchEdit);
        searchEdit->hide();

        layout->addWidget(begTag, 1, 0, 1, 1);
        layout->addWidget(begTimeEdit, 1, 1, 1, 3);
        layout->addWidget(endTag, 1, 4, 1, 1);
        layout->addWidget(endTimeEdit, 1, 5, 1, 3);
        begTag->show();
        begTimeEdit->show();
        endTag->show();
        endTimeEdit->show();
    });

    connect(searchBtn, &QPushButton::clicked, this, &HistoryMessageWidget::ClickSearchBtn);

#if TEST_UI
    for (int i = 0; i < 30; ++i)
    {
        UserInfo sender;
        sender.userId = "";
        sender.nickname = "SnowK" + QString::number(i);
        sender.avatar = QIcon(":/resource/image/defaultAvatar.png");
        sender.description = "";
        sender.phone = "18351958129";
        Message message = Message::MakeMessage(MessageType::TEXT_TYPE, "", sender,
                                               QString("Message content" + QString::number(i)).toUtf8(), "");
        this->AddHistoryMessage(message);
    }
#endif
}

void HistoryMessageWidget::AddHistoryMessage(const Message &message)
{
    HistoryItem* item = HistoryItem::MakeHistoryItem(message);
    container->layout()->addWidget(item);
}

void HistoryMessageWidget::Clear()
{
    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(container->layout());
    for (int i = layout->count() - 1; i >= 0; --i)
    {
        QWidget*  w= layout->itemAt(i)->widget();
        if (w == nullptr)
        {
            continue;
        }

        layout->removeWidget(w);
        w->deleteLater();
    }
}

void HistoryMessageWidget::ClickSearchBtn()
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    connect(dataCenter, &DataCenter::SearchMessageDone, this,
            &HistoryMessageWidget::ClickSearchBtnDone, Qt::UniqueConnection);

    if (keyRadioBtn->isChecked())
    {
        const QString& searchKey = searchEdit->text();
        if (searchKey.isEmpty())
        {
            return;
        }

        dataCenter->SearchMessageAsync(searchKey);
    }
    else // timeRadioBtn
    {
        auto begTime = begTimeEdit->dateTime();
        auto endTime = endTimeEdit->dateTime();
        if (begTime >= endTime)
        {
            Toast::ShowMessage("Time error");
            return;
        }

        dataCenter->SearchMessageByTimeAsync(begTime, endTime);
    }
}

void HistoryMessageWidget::ClickSearchBtnDone()
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    QList<Message>* messageResult = dataCenter->GetSearchMessageResult();
    if (messageResult == nullptr)
    {
        return;
    }
    this->Clear();

    for (const Message& m : *messageResult)
    {
        this->AddHistoryMessage(m);
    }
}

void HistoryMessageWidget::InitScrollArea(QGridLayout *layout)
{
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidgetResizable(true);
    scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 2px; "
                                                   "background-color: rgb(255, 255, 255); }");
    scrollArea->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal { height: 0; }");
    scrollArea->setStyleSheet("QScrollArea { border: none; }");

    container = new QWidget();
    scrollArea->setWidget(container);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(10);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->setAlignment(Qt::AlignTop);
    container->setLayout(vlayout);

    layout->addWidget(scrollArea, 2, 0, 1, 9);
}

////////////////////////////////////////////////////////////////////
/// ImageButton
////////////////////////////////////////////////////////////////////

ImageButton::ImageButton(const QString &fileId, const QByteArray &content)
    : fileId(fileId)
{
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setStyleSheet("QPushButton { border: none; }");

    if (!content.isEmpty())
    {
        this->UpdateUI(fileId, content);
    }
    else
    {
        DataCenter* dataCenter = DataCenter::GetInstance();
        connect(dataCenter, &DataCenter::GetSingleFileDone, this, &ImageButton::UpdateUI);
        dataCenter->GetSingleFileAsync(fileId);
    }
}

void ImageButton::UpdateUI(const QString &fileId, const QByteArray &content)
{
    if (this->fileId != fileId)
    {
        return;
    }

    QImage image;
    image.loadFromData(content);
    int width = image.width();
    int height = image.height();

    // If the image size is large, it needs to be scaled
    if (image.width() >= 300)
    {
        width = 300;
        height = ((double)image.height() / image.width()) * width;
    }

    this->resize(width, height);
    this->setIconSize(QSize(width, height));
    QPixmap pixmap = QPixmap::fromImage(image);
    this->setIcon(QIcon(pixmap));
}

////////////////////////////////////////////////////////////////////
/// FileLabel
////////////////////////////////////////////////////////////////////

FileLabel::FileLabel(const QString &fileId, const QString &fileName)
    : fileId(fileId)
    , fileName(fileName)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setText("[File] " + fileName);
    this->setWordWrap(true);
    this->adjustSize(); // Automatically adjust the size to display the text content
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    DataCenter* dataCenter = DataCenter::GetInstance();
    connect(dataCenter, &DataCenter::GetSingleFileDone, this, &FileLabel::GetContentDone);
    dataCenter->GetSingleFileAsync(this->fileId);
}

void FileLabel::GetContentDone(const QString &fileId, const QByteArray &fileContent)
{
    if (fileId != this->fileId)
    {
        return;
    }

    this->content = fileContent;
    this->loadDone = true;
}

void FileLabel::mousePressEvent(QMouseEvent *event)
{
    (void) event;
    if (!this->loadDone)
    {
        Toast::ShowMessage("File content is loading, please try later");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, "Save as", QDir::homePath(), "*");
    if (filePath.isEmpty())
    {
        LOG() << "User canceled save";
        return;
    }

    model::Util::WriteByteArrayToFile(filePath, content);
}

////////////////////////////////////////////////////////////////////
/// SpeechLabel
////////////////////////////////////////////////////////////////////

SpeechLabel::SpeechLabel(const QString &fileId)
    : fileId(fileId)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setText("[Speech]");
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setWordWrap(true);
    this->adjustSize();

    DataCenter* dataCenter = DataCenter::GetInstance();
    connect(dataCenter, &DataCenter::GetSingleFileDone, this, &SpeechLabel::GetContentDone);
    dataCenter->GetSingleFileAsync(fileId);
}

void SpeechLabel::GetContentDone(const QString &fileId, const QByteArray &content)
{
    if (fileId != this->fileId)
    {
        return;
    }

    this->content = content;
    this->loadDone = true;
}

void SpeechLabel::mousePressEvent(QMouseEvent *event)
{
    (void) event;
    if (!this->loadDone)
    {
        Toast::ShowMessage("File content is loading, please try later");
        return;
    }

    SoundRecorder* soundRecorder = SoundRecorder::GetInstance();
    soundRecorder->StartPlay(this->content);
}