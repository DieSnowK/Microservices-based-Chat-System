#ifndef USERINFOWIDGET_H
#define USERINFOWIDGET_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include "data.hpp"

using model::UserInfo;

class UserInfoWidget : public QDialog
{
    Q_OBJECT

public:
    UserInfoWidget(const UserInfo& userInfo, QWidget* parent);

    void InitSignalSlot();
    void ClickDeleteFriendBtn();
    void ClickApplyBtn();

private:
    const UserInfo& userInfo;

    QPushButton* avatarBtn;
    QLabel* idTag;
    QLabel* idLabel;
    QLabel* nameTag;
    QLabel* nameLabel;
    QLabel* phoneTag;
    QLabel* phoneLabel;

    QPushButton* applyBtn;
    QPushButton* sendMessageBtn;
    QPushButton* deleteFriendBtn;
};

#endif // USERINFOWIDGET_H
