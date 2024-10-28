#include "sessiondetailwidget.h"

/////////////////////////////////////////////
/// AvatarItem
/////////////////////////////////////////////

AvatarItem::AvatarItem(const QIcon &avatar, const QString &name)
{
    this->setFixedSize(70, 80);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignHCenter);
    this->setLayout(layout);

    avatarBtn = new QPushButton();
    avatarBtn->setFixedSize(45, 45);
    avatarBtn->setIconSize(QSize(45, 45));
    avatarBtn->setIcon(avatar);
    avatarBtn->setStyleSheet("QPushButton { border: none; }");

    nameLabel = new QLabel();
    nameLabel->setText(name);
    QFont font("微软雅黑", 12);
    nameLabel->setFont(font);
    nameLabel->setAlignment(Qt::AlignCenter);

    // Do a "truncation" operation on the name
    const int MAX_WIDTH = 65;
    QFontMetrics metrics(font);
    int totalWidth = metrics.horizontalAdvance(name);
    if (totalWidth >= MAX_WIDTH)
    {
        QString tail = "...";
        int tailWidth = metrics.horizontalAdvance(tail);
        int availableWidth = MAX_WIDTH - tailWidth;
        int availableSize = name.size() * ((double)availableWidth / totalWidth);
        QString newName = name.left(availableSize);
        nameLabel->setText(newName + tail);
    }

    layout->addWidget(avatarBtn);
    layout->addWidget(nameLabel);
}

QPushButton *AvatarItem::GetAvatar()
{
    return avatarBtn;
}

/////////////////////////////////////////////
/// SessionDetailWidget -> Single
/////////////////////////////////////////////

SessionDetailWidget::SessionDetailWidget(QWidget* parent, const UserInfo& userInfo)
    : QDialog(parent)
    , userInfo(userInfo)
{
    this->setWindowTitle("Session Details");
    this->setWindowIcon(QIcon(":/resource/image/logo.png"));
    this->setFixedSize(300, 300);
    this->setStyleSheet("QWidget { background-color: rgb(255, 255, 255); }");
    this->setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(10);
    layout->setContentsMargins(50, 0, 50, 0);
    this->setLayout(layout);

    // Add a "Create Group Chat" button
    AvatarItem* createGroupBtn = new AvatarItem(QIcon(":/resource/image/cross.png"), "Add");
    layout->addWidget(createGroupBtn, 0, 0);

#if TEST_UI
    AvatarItem* currentUser = new AvatarItem(QIcon(":/resource/image/defaultAvatar.png"), "DieSnowK666");
    layout->addWidget(currentUser, 0, 1);
#endif

    AvatarItem* currentUser = new AvatarItem(userInfo.avatar, userInfo.nickname);
    layout->addWidget(currentUser, 0, 1);

    deleteFriendBtn = new QPushButton();
    deleteFriendBtn->setFixedHeight(50);
    deleteFriendBtn->setText("Delete Friend");
    QString style = "QPushButton { border: 1px solid rgb(90, 90, 90); border-radius: 5px; } ";
    style += "QPushButton:pressed { background-color: rgb(235, 235, 235); }";
    deleteFriendBtn->setStyleSheet(style);
    layout->addWidget(deleteFriendBtn, 1, 0, 1, 3);

    connect(createGroupBtn->GetAvatar(), &QPushButton::clicked, this, [=]()
    {
        // ChooseFriendDialog* chooseFriendDialog = new ChooseFriendDialog(this, userInfo.userId);
        ChooseFriendDialog* chooseFriendDialog = new ChooseFriendDialog(this);
        chooseFriendDialog->exec();
    });

    connect(deleteFriendBtn, &QPushButton::clicked, this, &SessionDetailWidget::ClickDeleteFriendBtn);
}

void SessionDetailWidget::ClickDeleteFriendBtn()
{

}

