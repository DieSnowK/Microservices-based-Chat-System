#include "mainwidget.h"
#include "./ui_mainwidget.h"

#include <QHBoxLayout>
#include <QGridLayout>

#include "sessionfriendarea.h"

MainWidget* MainWidget::instance = nullptr;

MainWidget* MainWidget::GetInstance()
{
    if(instance == nullptr)
    {
        instance = new MainWidget();
    }

    return instance;
}

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    this->setWindowTitle("SnowK's Chat System");
    this->setWindowIcon(QIcon(":/resource/image/logo.png"));

    InitMainWindow();
    InitLeftWindow();
    InitMidWindow();
    InitRightWindow();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::InitMainWindow()
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    windowLeft = new QWidget();
    windowMid = new QWidget();
    windowRight = new QWidget();
    
    windowLeft->setFixedWidth(70);
    windowMid->setFixedWidth(310);
    windowRight->setMinimumWidth(800);

    windowLeft->setStyleSheet("QWidget { background-color: rgb(46, 46, 46); }");
    windowMid->setStyleSheet("QWidget { background-color: rgb(247, 247, 247); }");
    windowRight->setStyleSheet("QWidget { background-color: rgb(245, 245, 245); }");

    layout->addWidget(windowLeft);
    layout->addWidget(windowMid);
    layout->addWidget(windowRight);
}

void MainWidget::InitLeftWindow()
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(20);
    layout->setContentsMargins(0, 50, 0, 0);
    windowLeft->setLayout(layout);

    userAvatar = new QPushButton();
    userAvatar->setFixedSize(45, 45);
    userAvatar->setIconSize(QSize(45, 45));
    userAvatar->setIcon(QIcon(":/resource/image/defaultAvatar.png"));
    userAvatar->setStyleSheet("QPushButton { background-color: transparent; }");
    layout->addWidget(userAvatar, 1, Qt::AlignTop | Qt::AlignHCenter);

    sessionTabBtn = new QPushButton();
    sessionTabBtn->setFixedSize(45, 45);
    sessionTabBtn->setIconSize(QSize(30, 30));
    sessionTabBtn->setIcon(QIcon(":/resource/image/session_active.png"));
    sessionTabBtn->setStyleSheet("QPushButton { background-color: transparent; }");
    layout->addWidget(sessionTabBtn, 1, Qt::AlignTop | Qt::AlignHCenter);

    friendTabBtn = new QPushButton();
    friendTabBtn->setFixedSize(45, 45);
    friendTabBtn->setIconSize(QSize(30, 30));
    friendTabBtn->setIcon(QIcon(":/resource/image/friend_inactive.png"));
    friendTabBtn->setStyleSheet("QPushButton { background-color: transparent; }");
    layout->addWidget(friendTabBtn, 1, Qt::AlignTop | Qt::AlignHCenter);

    applyTabBtn = new QPushButton();
    applyTabBtn->setFixedSize(45, 45);
    applyTabBtn->setIconSize(QSize(30, 30));
    applyTabBtn->setIcon(QIcon(":/resource/image/apply_inactive.png"));
    applyTabBtn->setStyleSheet("QPushButton { background-color: transparent; }");
    layout->addWidget(applyTabBtn, 1, Qt::AlignTop | Qt::AlignHCenter);

    layout->addStretch(20);

    InitSignalSlot();
}

void MainWidget::InitMidWindow()
{
    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 20, 0, 0);
    layout->setHorizontalSpacing(0);
    layout->setVerticalSpacing(10);
    windowMid->setLayout(layout);

    searchEdit = new QLineEdit();
    searchEdit->setFixedHeight(30);
    searchEdit->setPlaceholderText("Search");
    searchEdit->setStyleSheet("QLineEdit { border-radius: 5px; background-color: rgb(226, 226, 226); padding-left: 5px; }");

    addFriendBtn = new QPushButton();
    addFriendBtn->setFixedSize(30, 30);
    addFriendBtn->setIcon(QIcon(":/resource/image/cross.png"));
    QString style = "QPushButton { border-radius: 5px; background-color: rgb(226, 226, 226); }";
    style += " QPushButton:pressed { background-color: rgb(240, 240, 240); }";
    addFriendBtn->setStyleSheet(style);

    SessionFriendArea* sessionFriendArea = new SessionFriendArea();

    // For more flexible control of margins, only the row of the
        // search box button is affected, not the row of the list below
    // Create a blank widget to fill into the Layout Manager
    QWidget* spacer1 = new QWidget();
    spacer1->setFixedWidth(10);
    QWidget* spacer2 = new QWidget();
    spacer2->setFixedWidth(10);
    QWidget* spacer3 = new QWidget();
    spacer3->setFixedWidth(10);

    layout->addWidget(spacer1, 0, 0);
    layout->addWidget(searchEdit, 0, 1);
    layout->addWidget(spacer2, 0, 2);
    layout->addWidget(addFriendBtn, 0, 3);
    layout->addWidget(spacer3, 0, 4);
    layout->addWidget(sessionFriendArea, 1, 0, 1, 5);
}

void MainWidget::InitRightWindow()
{
    
}

void MainWidget::InitSignalSlot()
{
    connect(sessionTabBtn, &QPushButton::clicked, this, &MainWidget::SwitchTabToSession);     
    connect(friendTabBtn, &QPushButton::clicked, this, &MainWidget::SwitchTabToFriend);     
    connect(applyTabBtn, &QPushButton::clicked, this, &MainWidget::SwitchTabToApply);     
}

void MainWidget::SwitchTabToSession()
{
    activeTab = ActiveTab::SESSION_LIST;
    
    sessionTabBtn->setIcon(QIcon(":/resource/image/session_active.png"));
    friendTabBtn->setIcon(QIcon(":/resource/image/friend_inactive.png"));
    applyTabBtn->setIcon(QIcon(":/resource/image/apply_inactive.png"));

    this->LoadSessionList();
}

void MainWidget::SwitchTabToFriend()
{
    activeTab = ActiveTab::FRIEND_LIST;
    
    friendTabBtn->setIcon(QIcon(":/resource/image/friend_active.png"));
    sessionTabBtn->setIcon(QIcon(":/resource/image/session_inactive.png"));
    applyTabBtn->setIcon(QIcon(":/resource/image/apply_inactive.png"));

    this->LoadFriendList();
}

void MainWidget::SwitchTabToApply()
{
    activeTab = ActiveTab::APPLY_LIST;
    
    applyTabBtn->setIcon(QIcon(":/resource/image/apply_active.png"));
    friendTabBtn->setIcon(QIcon(":/resource/image/friend_inactive.png"));
    sessionTabBtn->setIcon(QIcon(":/resource/image/session_inactive.png"));

    this->LoadApplyList();
}

void MainWidget::LoadSessionList()
{

}

void MainWidget::LoadFriendList()
{

}

void MainWidget::LoadApplyList()
{

}
