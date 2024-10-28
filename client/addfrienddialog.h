#ifndef ADDFRIENDDIALOG_H
#define ADDFRIENDDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>

#include "model/data.hpp"

using model::UserInfo;

//////////////////////////////////////
/// AddFriendDialog
//////////////////////////////////////

class AddFriendDialog : public QDialog
{
    Q_OBJECT

public:
    AddFriendDialog(QWidget* parent);

    void AddResult(const UserInfo& userInfo);

private:
    QLineEdit* searchEdit;
    QGridLayout* layout;
    QWidget* resultContainer;
};

#endif // ADDFRIENDDIALOG_H
