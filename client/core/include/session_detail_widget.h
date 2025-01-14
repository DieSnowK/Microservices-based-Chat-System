#ifndef SESSIONDETAILWIDGET_H
#define SESSIONDETAILWIDGET_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "data.hpp"

using model::UserInfo;

/////////////////////////////////////////////
/// AvatarItem -> Avatar + Label
/////////////////////////////////////////////

class AvatarItem : public QWidget
{
    Q_OBJECT

public:
    AvatarItem(const QIcon& avatar, const QString& name);

    QPushButton* GetAvatar();

private:
    QPushButton* avatarBtn;
    QLabel* nameLabel;
};

/////////////////////////////////////////////
/// SessionDetailWidget -> Single
/////////////////////////////////////////////

class SessionDetailWidget : public QDialog
{
    Q_OBJECT

public:
    SessionDetailWidget(QWidget* parent, const UserInfo& userInfo);

    void ClickDeleteFriendBtn();

private:
    QPushButton* deleteFriendBtn;
    UserInfo userInfo;
};

#endif // SESSIONDETAILWIDGET_H
