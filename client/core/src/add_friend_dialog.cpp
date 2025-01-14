#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>

#include "add_friend_dialog.h"
#include "debug.h"
#include "datacenter.h"

using model::DataCenter;

//////////////////////////////////////
/// FriendResultItem
//////////////////////////////////////

FriendResultItem::FriendResultItem(const UserInfo &userInfo)
    : userInfo(userInfo)
{
    this->setFixedHeight(70);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QGridLayout* layout = new QGridLayout();
    layout->setVerticalSpacing(0);
    layout->setHorizontalSpacing(10);
    layout->setContentsMargins(0, 0, 20, 0);
    this->setLayout(layout);

    QPushButton* avatarBtn = new QPushButton();
    avatarBtn->setFixedSize(50, 50);
    avatarBtn->setIconSize(QSize(50, 50));
    avatarBtn->setIcon(userInfo.avatar);

    QLabel* nameLabel = new QLabel();
    nameLabel->setFixedHeight(35);
    nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    nameLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: 700;}");
    nameLabel->setText(userInfo.nickname);

    QLabel* descLabel = new QLabel();
    descLabel->setFixedHeight(35);
    descLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    descLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    descLabel->setStyleSheet("QLabel { font-size: 14px; }");
    descLabel->setText(userInfo.description);

    addBtn = new QPushButton();
    addBtn->setFixedSize(100, 40);
    addBtn->setText("Add Friend");
    QString btnStyle = "QPushButton { border: none; background-color: rgb(137, 217, 97); "
                       "color: rgb(255, 255, 255); border-radius: 10px;} ";
    btnStyle += "QPushButton:pressed { background-color: rgb(200, 200, 200); }";
    addBtn->setStyleSheet(btnStyle);

    layout->addWidget(avatarBtn, 0, 0, 2, 1);
    layout->addWidget(nameLabel, 0, 1);
    layout->addWidget(descLabel, 1, 1);
    layout->addWidget(addBtn, 0, 2, 2, 1);

    connect(addBtn, &QPushButton::clicked, this, &FriendResultItem::ClickAddBtn);
}

void FriendResultItem::ClickAddBtn()
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    dataCenter->AddFriendApplyAsync(this->userInfo.userId);

    addBtn->setEnabled(false);
    addBtn->setText("Already applied");
    addBtn->setStyleSheet("QPushButton { border:none; color: rgb(255, 255, 255); "
                          "background-color: rgb(200, 200, 200); border-radius: 10px;}");
}

//////////////////////////////////////
/// AddFriendDialog
//////////////////////////////////////

AddFriendDialog::AddFriendDialog(QWidget* parent)
    : QDialog(parent)
{
    this->setFixedSize(500, 500);
    this->setWindowTitle("Add Friend");
    this->setWindowIcon(QIcon(":/resource/image/logo.png"));
    this->setStyleSheet("QDialog {background-color: rgb(255, 255, 255); }");
    this->setAttribute(Qt::WA_DeleteOnClose);

    layout = new QGridLayout();
    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 0);
    this->setLayout(layout);

    searchEdit = new QLineEdit();
    searchEdit->setFixedHeight(50);
    searchEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QString style = "QLineEdit { border: none; border-radius: 10px; font-size: 16px; "
                    "background-color: rgb(240, 240, 240); padding-left: 5px;}";
    searchEdit->setStyleSheet(style);
    searchEdit->setPlaceholderText("Search by phone/userId/nickname");
    layout->addWidget(searchEdit, 0, 0, 1, 8);

    QPushButton* searchBtn = new QPushButton();
    searchBtn->setFixedSize(50, 50);
    searchBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    searchBtn->setIconSize(QSize(30, 30));
    searchBtn->setIcon(QIcon(":/resource/image/search.png"));
    QString btnStyle = "QPushButton { border: none; "
                       "background-color: rgb(240, 240, 240); border-radius: 10px; }";
    btnStyle += "QPushButton:hover { background-color: rgb(220, 220, 220); } "
                "QPushButton:pressed { background-color: rgb(200, 200, 200); } ";
    searchBtn->setStyleSheet(btnStyle);
    layout->addWidget(searchBtn, 0, 8, 1, 1);

    InitResultArea();

#if TEST_UI
    QIcon avatar(":/resource/image/defaultAvatar.png");
    for (int i = 0; i < 20; ++i)
    {
        UserInfo* userInfo = new UserInfo();
        userInfo->userId = QString::number(1000 + i);
        userInfo->nickname = "SnowK" + QString::number(i);
        userInfo->description = "Cool Boy~";
        userInfo->avatar = avatar;
        this->AddResult(*userInfo);
    }
#endif

    connect(searchBtn, &QPushButton::clicked, this, &AddFriendDialog::ClickSearchBtn);
}

void AddFriendDialog::InitResultArea()
{
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidgetResizable(true);
    scrollArea->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {height: 0;} ");
    scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 2px; "
                                                   "background-color: rgb(255, 255, 255);}");
    scrollArea->setStyleSheet("QScrollArea { border: none; }");
    layout->addWidget(scrollArea, 1, 0, 1, 9);

    resultContainer = new QWidget();
    resultContainer->setObjectName("resultContainer");
    resultContainer->setStyleSheet("#resultContainer { background-color: rgb(255, 255, 255); } ");
    scrollArea->setWidget(resultContainer);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0, 0, 0, 0);
    resultContainer->setLayout(vlayout);
}

void AddFriendDialog::AddResult(const UserInfo &userInfo)
{
    FriendResultItem* item = new FriendResultItem(userInfo);
    resultContainer->layout()->addWidget(item);
}

void AddFriendDialog::Clear()
{
    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(resultContainer->layout());
    for (int i = layout->count() - 1; i >= 0; --i)
    {
        QLayoutItem* layoutItem = layout->takeAt(i);
        if (layoutItem == nullptr || layoutItem->widget() == nullptr)
        {
            continue;
        }

        delete layoutItem->widget();
    }
}

void AddFriendDialog::SetSearchKey(const QString &searchKey)
{
    searchEdit->setText(searchKey);
}

void AddFriendDialog::ClickSearchBtn()
{
    const QString& text = searchEdit->text();
    if (text.isEmpty())
    {
        return;
    }

    DataCenter* dataCenter = DataCenter::GetInstance();
    connect(dataCenter, &DataCenter::SearchUserDone, this,
            &AddFriendDialog::ClickSearchBtnDone, Qt::UniqueConnection);
    dataCenter->SearchUserAsync(text);
}

void AddFriendDialog::ClickSearchBtnDone()
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    QList<UserInfo>* searchResult = dataCenter->GetSearchUserResult();
    if (searchResult == nullptr)
    {
        return;
    }

    this->Clear();
    for (const auto& u : *searchResult)
    {
        this->AddResult(u);
    }
}


