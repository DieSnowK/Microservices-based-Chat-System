#include "mainwidget.h"
#include "./ui_mainwidget.h"
#include "debug.hpp"

using model::ChatSessionInfo;

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
    InitSignalSlot();
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
    searchEdit->setStyleSheet("QLineEdit { border-radius: 5px; "
                              "background-color: rgb(226, 226, 226); padding-left: 5px; }");

    addFriendBtn = new QPushButton();
    addFriendBtn->setFixedSize(30, 30);
    addFriendBtn->setIcon(QIcon(":/resource/image/cross.png"));
    QString style = "QPushButton { border-radius: 5px; background-color: rgb(226, 226, 226); }";
    style += " QPushButton:pressed { background-color: rgb(240, 240, 240); }";
    addFriendBtn->setStyleSheet(style);

    sessionFriendArea = new SessionFriendArea();

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
    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->setAlignment(Qt::AlignTop);
    windowRight->setLayout(vlayout);

    QWidget* titleWidget = new QWidget();
    titleWidget->setFixedHeight(62);
    titleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    titleWidget->setObjectName("titleWidget");
    titleWidget->setStyleSheet("#titleWidget { border-bottom: 1px solid rgb(230, 230, 230); \
                                border-left: 1px solid rgb(230, 230, 230); }");
    vlayout->addWidget(titleWidget);

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setSpacing(0);
    hlayout->setContentsMargins(10, 0, 10, 0);
    titleWidget->setLayout(hlayout);

    sessionTitleLabel = new QLabel();
    sessionTitleLabel->setStyleSheet("QLabel { font-size: 22px; border-bottom: 1px solid rgb(230, 230, 230);}");
#if TEST_UI
    sessionTitleLabel->setText("DieSnowK");
#endif
    hlayout->addWidget(sessionTitleLabel);

    extraBtn = new QPushButton();
    extraBtn->setFixedSize(30, 30);
    extraBtn->setIconSize(QSize(30, 30));
    extraBtn->setIcon(QIcon(":/resource/image/more.png"));
    extraBtn->setStyleSheet("QPushButton { border:none; background-color: rgb(245, 245, 245); } \
                             QPushButton:pressed { background-color: rgb(220, 220, 220); }");
    hlayout->addWidget(extraBtn);

    messageShowArea = new MessageShowArea();
    vlayout->addWidget(messageShowArea);

    messageEditArea = new MessageEditArea();
    vlayout->addWidget(messageEditArea, 0, Qt::AlignBottom);
}

void MainWidget::InitSignalSlot()
{
    connect(sessionTabBtn, &QPushButton::clicked, this, &MainWidget::SwitchTabToSession);
    connect(friendTabBtn, &QPushButton::clicked, this, &MainWidget::SwitchTabToFriend);
    connect(applyTabBtn, &QPushButton::clicked, this, &MainWidget::SwitchTabToApply);

    connect(userAvatar, &QPushButton::clicked, this, [=]()
    {
        SelfInfoWidget* selfInfoWidget = new SelfInfoWidget(this);
        selfInfoWidget->exec();         // A modal dialog box pops up
        // selfInfoWidget->show();      // Pop up non-modal
    });

    connect(extraBtn, &QPushButton::clicked, this, [=]()
    {
#if TEST_GROUP_SESSION_DETAIL
        bool isSingleChat = false;
#else
        bool isSingleChat = true;
#endif

        // ChatSessionInfo* chatSessionInfo = dataCenter->findChatSessionById(dataCenter->getCurrentChatSessionId());
        // if (chatSessionInfo == nullptr) {
        //     LOG() << "The current session does not exist, the Session Details dialog box cannot be displayed";
        //     return;
        // }

        // bool isSingleChat = chatSessionInfo->userId != "";

        if (isSingleChat)
        {
            // UserInfo* userInfo = dataCenter->findFriendById(chatSessionInfo->userId);
            // if (userInfo == nullptr) {
            //     LOG() << "The user for a one-to-one chat session does not exist, the session details window cannot be displayed";
            //     return;
            // }
            // SessionDetailWidget* sessionDetailWidget = new SessionDetailWidget(this, *userInfo);
            // sessionDetailWidget->exec();
        }
        else
        {
            GroupSessionDetailWidget* groupSessionDetailWidget = new GroupSessionDetailWidget(this);
            groupSessionDetailWidget->exec();
        }
    });

    connect(addFriendBtn, &QPushButton::clicked, this, [=]()
    {
        AddFriendDialog* addFriendDialog = new AddFriendDialog(this);
        addFriendDialog->exec();
    });

    // TODO 考虑优化
    connect(searchEdit, &QLineEdit::textEdited, this, [=]()
    {
        const QString& searchKey = searchEdit->text();
        AddFriendDialog* addFriendDialog = new AddFriendDialog(this);
        addFriendDialog->SetSearchKey(searchKey);
        searchEdit->setText("");
        addFriendDialog->exec();
    });
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
