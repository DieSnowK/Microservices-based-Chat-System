#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>

#include "message_edit_area.h"
#include "mainwidget.h"
#include "data.hpp"
#include "sound_recorder.h"
#include "datacenter.h"
#include "toast.h"

using model::DataCenter;

MessageEditArea::MessageEditArea(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedHeight(200);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(10, 0, 10, 10);
    this->setLayout(vlayout);

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setSpacing(0);
    hlayout->setContentsMargins(10, 0, 0, 0);
    hlayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    vlayout->addLayout(hlayout);

    QSize btnSize(35, 35);
    QSize iconSize(25, 25);
    QString btnStyle = "QPushButton { background-color: rgb(245, 245, 245); border: none; } "
                       "QPushButton:pressed { background-color: rgb(255, 255, 255); }";

    sendImageBtn = new QPushButton();
    sendImageBtn->setFixedSize(btnSize);
    sendImageBtn->setIconSize(iconSize);
    sendImageBtn->setIcon(QIcon(":/resource/image/image.png"));
    sendImageBtn->setStyleSheet(btnStyle);
    hlayout->addWidget(sendImageBtn);

    sendFileBtn = new QPushButton();
    sendFileBtn->setFixedSize(btnSize);
    sendFileBtn->setIconSize(iconSize);
    sendFileBtn->setIcon(QIcon(":/resource/image/file.png"));
    sendFileBtn->setStyleSheet(btnStyle);
    hlayout->addWidget(sendFileBtn);

    sendSpeechBtn = new QPushButton();
    sendSpeechBtn->setFixedSize(btnSize);
    sendSpeechBtn->setIconSize(iconSize);
    sendSpeechBtn->setIcon(QIcon(":/resource/image/sound.png"));
    sendSpeechBtn->setStyleSheet(btnStyle);
    hlayout->addWidget(sendSpeechBtn);

    showHistoryBtn = new QPushButton();
    showHistoryBtn->setFixedSize(btnSize);
    showHistoryBtn->setIconSize(iconSize);
    showHistoryBtn->setIcon(QIcon(":/resource/image/history.png"));
    showHistoryBtn->setStyleSheet(btnStyle);
    hlayout->addWidget(showHistoryBtn);

    textEdit = new QPlainTextEdit();
    textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    textEdit->setStyleSheet("QPlainTextEdit { border: none; "
                            "background-color: transparent; font-size: 14px; padding: 10px; }");
    textEdit->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 2px; "
                                                 "background-color: rgb(45, 45, 45); }");
    vlayout->addWidget(textEdit);

    tipLabel = new QLabel();
    tipLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tipLabel->setText("Recording...");
    tipLabel->setAlignment(Qt::AlignCenter);
    tipLabel->setFont(QFont("微软雅黑", 24, 600));
    vlayout->addWidget(tipLabel);
    tipLabel->hide();

    sendTextBtn = new QPushButton();
    sendTextBtn->setText("Send");
    sendTextBtn->setFixedSize(120, 40);
    QString style = "QPushButton { font-size: 16px; color: rgb(7, 193, 96); border: none; "
                    "background-color: rgb(233, 233, 233); border-radius: 10px; } ";
    style += "QPushButton:hover { background-color: rgb(210, 210, 210); }";
    style += "QPushButton:pressed { background-color: rgb(190, 190, 190); }";
    sendTextBtn->setStyleSheet(style);
    vlayout->addWidget(sendTextBtn, 0, Qt::AlignRight | Qt::AlignVCenter);

    InitSignalSlot();
}

void MessageEditArea::InitSignalSlot()
{
    DataCenter* dataCenter = DataCenter::GetInstance();

    connect(showHistoryBtn, &QPushButton::clicked, this, [=]()
    {
        if (dataCenter->GetCurrentChatSessionId().isEmpty())
        {
            return;
        }

        HistoryMessageWidget* historyMessageWidget = new HistoryMessageWidget(this);
        historyMessageWidget->exec();
    });

    connect(sendTextBtn, &QPushButton::clicked, this, &MessageEditArea::SendTextMessage);
    connect(dataCenter, &DataCenter::SendMessageDone, this, &MessageEditArea::AddSelfMessage);
    connect(dataCenter, &DataCenter::ReceiveMessageDone, this, &MessageEditArea::AddOtherMessage);

    connect(sendImageBtn, &QPushButton::clicked, this, &MessageEditArea::ClickSendImageBtn);
    connect(sendFileBtn, &QPushButton::clicked, this, &MessageEditArea::ClickSendFileBtn);

    connect(sendSpeechBtn, &QPushButton::pressed, this, &MessageEditArea::SoundRecordPressed);
    connect(sendSpeechBtn, &QPushButton::released, this, &MessageEditArea::SoundRecordReleased);

    SoundRecorder* soundRecorder = SoundRecorder::GetInstance();
    connect(soundRecorder, &SoundRecorder::SoundRecordDone, this, &MessageEditArea::SendSpeech);
}

void MessageEditArea::SendTextMessage()
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    if (dataCenter->GetCurrentChatSessionId().isEmpty())
    {
        LOG() << "No conversations are currently selected and no messages will be sent";
        Toast::ShowMessage("No conversations are currently selected and no messages will be sent");
        return;
    }

    // trim: Remove the blank symbols on both sides of the string
    const QString& content = textEdit->toPlainText().trimmed();
    if (content.isEmpty())
    {
        LOG() << "The input box is empty";
        return;
    }

    textEdit->setPlainText("");

    dataCenter->SendTextMessageAsync(dataCenter->GetCurrentChatSessionId(), content);
}

void MessageEditArea::AddSelfMessage(MessageType messageType, const QByteArray &content, const QString &extraInfo)
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    const QString& currentChatSessionId = dataCenter->GetCurrentChatSessionId();

    Message message = Message::MakeMessage(messageType, currentChatSessionId,
                                           *dataCenter->GetMyself(), content, extraInfo);
    dataCenter->AddMessage(message);

    // Display this new message in the messageShowArea
    MainWidget* mainWidget = MainWidget::GetInstance();
    MessageShowArea* messageShowArea = mainWidget->GetMessageShowArea();
    messageShowArea->AddMessage(false, message);
    messageShowArea->ScrollToEnd();

    emit dataCenter->UpdateLastMessage(currentChatSessionId);
}

void MessageEditArea::AddOtherMessage(const Message &message)
{
    MainWidget* mainWidget = MainWidget::GetInstance();
    MessageShowArea* messageShowArea = mainWidget->GetMessageShowArea();

    messageShowArea->AddMessage(true, message);
    messageShowArea->ScrollToEnd();

    Toast::ShowMessage("Received a new message");
}

void MessageEditArea::ClickSendImageBtn()
{
    DataCenter* dataCenter = DataCenter::GetInstance();

    if (dataCenter->GetCurrentChatSessionId().isEmpty())
    {
        Toast::ShowMessage("You haven't selected any sessions yet");
        return;
    }

    QString filter = "Image Files (*.png *.jpg *.jpeg)";
    QString imagePath = QFileDialog::getOpenFileName(this, "Select picture", QDir::homePath(), filter);
    if (imagePath.isEmpty())
    {
        LOG() << "User deselects image";
        return;
    }

    QByteArray imageContent = model::Util::LoadFileToByteArray(imagePath);
    dataCenter->SendImageMessageAsync(dataCenter->GetCurrentChatSessionId(), imageContent);
}

void MessageEditArea::ClickSendFileBtn()
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    if (dataCenter->GetCurrentChatSessionId().isEmpty())
    {
        Toast::ShowMessage("You haven't selected any sessions yet");
        return;
    }

    QString filter = "*";
    QString path = QFileDialog::getOpenFileName(this, "Select file", QDir::homePath(), filter);
    if (path.isEmpty())
    {
        LOG() << "User deselects file";
        return;
    }

    // 1.Read the file content. Large files are not considered here for the time being.
        // If it is for large files, write a special network communication interface
            // interface to achieve the "slice transmission" effect
    QByteArray content = model::Util::LoadFileToByteArray(path);

    // 2.Get the file name
    QFileInfo fileInfo(path);
    const QString& fileName = fileInfo.fileName();

    // 3.Send message
    dataCenter->SendFileMessageAsync(dataCenter->GetCurrentChatSessionId(), fileName, content);
}

void MessageEditArea::SoundRecordPressed()
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    if (dataCenter->GetCurrentChatSessionId().isEmpty())
    {
        LOG() << "You haven't selected any sessions yet";
        return;
    }

    sendSpeechBtn->setIcon(QIcon(":/resource/image/sound_active.png"));

    SoundRecorder* soundRecorder = SoundRecorder::GetInstance();
    soundRecorder->StartRecord();

    tipLabel->show();
    textEdit->hide();
}

void MessageEditArea::SoundRecordReleased()
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    if (dataCenter->GetCurrentChatSessionId().isEmpty())
    {
        LOG() << "You haven't selected any sessions yet";
        return;
    }

    sendSpeechBtn->setIcon(QIcon(":/resource/image/sound.png"));

    SoundRecorder* soundRecorder = SoundRecorder::GetInstance();
    soundRecorder->StopRecord();

    tipLabel->hide();
    textEdit->show();
}

void MessageEditArea::SendSpeech(const QString &path)
{
    QByteArray content = model::Util::LoadFileToByteArray(path);
    if (content.isEmpty())
    {
        LOG() << "Failed to load voice file";
        return;
    }

    DataCenter* dataCenter = DataCenter::GetInstance();
    dataCenter->SendSpeechMessageAsync(dataCenter->GetCurrentChatSessionId(), content);
}