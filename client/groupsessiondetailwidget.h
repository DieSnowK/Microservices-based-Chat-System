#ifndef GROUPSESSIONDETAILWIDGET_H
#define GROUPSESSIONDETAILWIDGET_H

#include <QDialog>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>

#include "sessiondetailwidget.h"

class GroupSessionDetailWidget : public QDialog
{
    Q_OBJECT

public:
    GroupSessionDetailWidget(QWidget* parent);

    void InitData();
    // void InitMembers(const QString& chatSessionId);

    void AddMember(AvatarItem* avatarItem);

private:
    QGridLayout* glayout;
    QLabel* groupNameLabel;

    // Represents the row and column where the AvatarItem to be added is currently located
    int curRow = 0;
    int curCol = 1;
};

#endif // GROUPSESSIONDETAILWIDGET_H
