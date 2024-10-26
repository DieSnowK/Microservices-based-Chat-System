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

    // 5. 对名字做 "截断操作"
    // const int MAX_WIDTH = 65;
    // QFontMetrics metrics(font);
    // int totalWidth = metrics.horizontalAdvance(name);
    // if (totalWidth >= MAX_WIDTH) {
    //     // 需要截断
    //     QString tail = "...";
    //     int tailWidth = metrics.horizontalAdvance(tail);
    //     int availableWidth = MAX_WIDTH - tailWidth;
    //     int availableSize = name.size() * ((double)availableWidth / totalWidth);
    //     QString newName = name.left(availableSize);
    //     nameLabel->setText(newName + tail);
    // }

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

}

