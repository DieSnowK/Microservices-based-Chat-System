#ifndef ADDFRIENDDIALOG_H
#define ADDFRIENDDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>

#include "model/data.hpp"

using model::UserInfo;

//////////////////////////////////////
/// FriendResultItem
//////////////////////////////////////

class FriendResultItem : public QWidget
{
    Q_OBJECT

public:
    FriendResultItem(const UserInfo& userInfo);
    void ClickAddBtn();

private:
    const UserInfo& userInfo;
    QPushButton* addBtn;
};


//////////////////////////////////////
/// AddFriendDialog
//////////////////////////////////////

class AddFriendDialog : public QDialog
{
    Q_OBJECT

public:
    AddFriendDialog(QWidget* parent);

    void InitResultArea();

    void AddResult(const UserInfo& userInfo);
    void Clear();

    void SetSearchKey(const QString& searchKey);

    // void ClickSearchBtn();
    // void ClickSearchBtnDone();

private:
    QLineEdit* searchEdit;
    QGridLayout* layout;
    QWidget* resultContainer;
};

#endif // ADDFRIENDDIALOG_H
