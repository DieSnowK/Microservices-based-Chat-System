#include "mainwidget.h"
#include "./ui_mainwidget.h"

#include <QHBoxLayout>

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
    layout->setContentMargins(0, 0, 0, 0);
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
    layout->setContentMargins(0, 50, 0, 0);
    windowLeft->setLayout(layout);

    userAvatar = new QPushButton();
    userAvatar->setFixedWidth(45, 45);
    userAvatar->setIconSize(QSize(45, 45));
    userAvatar->setIcon(QIcon(":/resource/image/defaultAvatar.png"));
    userAvatar->setStyleSheet("QPushButton { background-color: transparent; }");
    layout->addWidget(userAvatar, 1, Qt::AlignTop | Qt::AlignHCenter);

    sessionTabBtn = new QPushButton();
    sessionTabBtn->setFixedWidth(45, 45);
    sessionTabBtn->setIconSize(QSize(30, 30));
    sessionTabBtn->setIcon(QIcon(":/resource/image/session_active.png"));
    sessionTabBtn->setStyleSheet("QPushButton { background-color: transparent; }");
    layout->addWidget(sessionTabBtn, 1, Qt::AlignTop | Qt::AlignHCenter);

    friendTabBtn = new QPushButton();
    friendTabBtn->setFixedWidth(45, 45);
    friendTabBtn->setIconSize(QSize(30, 30));
    friendTabBtn->setIcon(QIcon(":/resource/image/friend_inactive.png"));
    friendTabBtn->setStyleSheet("QPushButton { background-color: transparent; }");
    layout->addWidget(friendTabBtn, 1, Qt::AlignTop | Qt::AlignHCenter);

    applyTabBtn = new QPushButton();
    applyTabBtn->setFixedWidth(45, 45);
    applyTabBtn->setIconSize(QSize(30, 30));
    applyTabBtn->setIcon(QIcon(":/resource/image/apply_inactive.png"));
    applyTabBtn->setStyleSheet("QPushButton { background-color: transparent; }");
    layout->addWidget(applyTabBtn, 1, Qt::AlignTop | Qt::AlignHCenter);

    layout->addStretch(20);
}

void MainWidget::InitMidWindow()
{
    
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

void MainWidget::LoadSessionList();
void MainWidget::LoadFriendList();
void MainWidget::LoadApplyList();
