#include "messageeditarea.h"

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

    // tipLabel = new QLabel();
    // tipLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // tipLabel->setText("Recording...");
    // tipLabel->setAlignment(Qt::AlignCenter);
    // tipLabel->setFont(QFont("微软雅黑", 24, 600));
    // vlayout->addWidget(tipLabel);
    // tipLabel->hide();

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

}
