#include "addfrienddialog.h"
#include "debug.hpp"



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

    // 5. 添加滚动区域
    // InitResultArea();

#if TEST_UI
    QIcon avatar(":/resource/image/defaultAvatar.png");
    for (int i = 0; i < 20; ++i)
    {
        // new 出来这个对象, 再往 addResult 中添加. FriendResultItem 中持有了 UserInfo 的 const 引用. 需要确保引用是有效的引用
        UserInfo* userInfo = new UserInfo();
        userInfo->userId = QString::number(1000 + i);
        userInfo->nickname = "张三" + QString::number(i);
        userInfo->description = "这是一段个性签名";
        userInfo->avatar = avatar;
        this->AddResult(*userInfo);
    }
#endif

    // connect(searchBtn, &QPushButton::clicked, this, &AddFriendDialog::clickSearchBtn);
}
