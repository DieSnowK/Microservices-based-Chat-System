#include <QMessageBox>
#include <QGridLayout>

#include "userinfo_widget.h"
#include "datacenter.h"
#include "mainwidget.h"

using namespace model;

UserInfoWidget::UserInfoWidget(const UserInfo& userInfo, QWidget* parent)
    : QDialog(parent)
    , userInfo(userInfo)
{
    this->setFixedSize(400, 200);
    this->setWindowTitle("User Infomation");
    this->setWindowIcon(QIcon(":/resource/image/logo.png"));
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(QCursor::pos());

    QGridLayout* layout = new QGridLayout();
    layout->setVerticalSpacing(10);
    layout->setHorizontalSpacing(20);
    layout->setContentsMargins(40, 20, 0, 0);
    layout->setAlignment(Qt::AlignTop);
    this->setLayout(layout);

    avatarBtn = new QPushButton();
    avatarBtn->setFixedSize(75, 75);
    avatarBtn->setIconSize(QSize(75, 75));
    avatarBtn->setIcon(userInfo.avatar);

    QString labelStyle = "QLabel { font-weight: 800; padding-left: 20px;}";
    QString btnStyle = "QPushButton { border: 1px solid rgb(100, 100, 100); "
                       "border-radius: 5px; background-color: rgb(240, 240, 240); }";
    btnStyle += "QPushButton:pressed { background-color: rgb(205, 205, 205); }";

    int width = 85;
    int height = 30;

    idTag = new QLabel();
    idTag->setText("Ordinal");
    idTag->setStyleSheet(labelStyle);
    idTag->setFixedSize(width, height);
    idTag->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    idLabel = new QLabel();
    idLabel->setText(userInfo.userId);
    idLabel->setFixedSize(width, height);
    idLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    nameTag = new QLabel();
    nameTag->setText("Nickname");
    nameTag->setStyleSheet(labelStyle);
    nameTag->setFixedSize(width, height);
    nameTag->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    nameLabel = new QLabel();
    nameLabel->setText(userInfo.nickname);
    nameLabel->setFixedSize(width, height);
    nameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    phoneTag = new QLabel();
    phoneTag->setText("Phone");
    phoneTag->setStyleSheet(labelStyle);
    phoneTag->setFixedSize(width, height);
    phoneTag->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    phoneLabel = new QLabel();
    phoneLabel->setText(userInfo.phone);
    phoneLabel->setFixedSize(width, height);
    phoneLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    applyBtn = new QPushButton();
    applyBtn->setText("Apply Friend");
    applyBtn->setFixedSize(width, height);
    applyBtn->setStyleSheet(btnStyle);
    applyBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    sendMessageBtn = new QPushButton();
    sendMessageBtn->setText("Send Message");
    sendMessageBtn->setFixedSize(width, height);
    sendMessageBtn->setStyleSheet(btnStyle);
    sendMessageBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    deleteFriendBtn = new QPushButton();
    deleteFriendBtn->setText("Delete Friend");
    deleteFriendBtn->setFixedSize(width, height);
    deleteFriendBtn->setStyleSheet(btnStyle);
    deleteFriendBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    layout->addWidget(avatarBtn, 0, 0, 3, 1);

    layout->addWidget(idTag, 0, 1);
    layout->addWidget(idLabel, 0, 2);

    layout->addWidget(nameTag, 1, 1);
    layout->addWidget(nameLabel, 1, 2);

    layout->addWidget(phoneTag, 2, 1);
    layout->addWidget(phoneLabel, 2, 2);

    layout->addWidget(applyBtn, 3, 0);
    layout->addWidget(sendMessageBtn, 3, 1);
    layout->addWidget(deleteFriendBtn, 3, 2);

    // Initialize the disabled relationship of the button
    DataCenter* dataCenter = DataCenter::GetInstance();
    auto* myFriend = dataCenter->FindFriendById(this->userInfo.userId);
    if (myFriend == nullptr)
    {
        sendMessageBtn->setEnabled(false);
        deleteFriendBtn->setEnabled(false);
    }
    else
    {
        applyBtn->setEnabled(false);
    }

    InitSignalSlot();
}

void UserInfoWidget::InitSignalSlot()
{
    connect(sendMessageBtn, &QPushButton::clicked, this, [=]()
    {
        MainWidget* mainWidget = MainWidget::GetInstance();
        mainWidget->SwitchToSession(userInfo.userId);

        this->close();
    });

    connect(deleteFriendBtn, &QPushButton::clicked, this, &UserInfoWidget::ClickDeleteFriendBtn);
    connect(applyBtn, &QPushButton::clicked, this, &UserInfoWidget::ClickApplyBtn);
}

void UserInfoWidget::ClickDeleteFriendBtn()
{
    auto result = QMessageBox::warning(this, "Confirm deletion", "Confirm to delete current friend?",
                                       QMessageBox::Ok | QMessageBox::Cancel);
    if (result != QMessageBox::Ok)
    {
        LOG() << "Delete friend cancel";
        return;
    }

    DataCenter* dataCenter = DataCenter::GetInstance();
    // Signal -> MainWidget
    dataCenter->DeleteFriendAsync(userInfo.userId);

    this->close();
}

void UserInfoWidget::ClickApplyBtn()
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    dataCenter->AddFriendApplyAsync(userInfo.userId);

    this->close();
}
