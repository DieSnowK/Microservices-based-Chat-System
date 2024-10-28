#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QGridLayout>

#include "sessionfriendarea.h"
#include "messageshowarea.h"
#include "messageeditarea.h"
#include "selfinfowidget.h"
#include "sessiondetailwidget.h"
#include "groupsessiondetailwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui 
{
    class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    ~MainWidget();
    static MainWidget* GetInstance();

    void InitMainWindow();
    void InitLeftWindow();
    void InitMidWindow();
    void InitRightWindow();

    void InitSignalSlot();

    void SwitchTabToSession();
    void SwitchTabToFriend();
    void SwitchTabToApply();

    void LoadSessionList();
    void LoadFriendList();
    void LoadApplyList();

private:
    MainWidget(QWidget *parent = nullptr);

private:
    Ui::MainWidget *ui;
    static MainWidget *instance;

    QWidget *windowLeft;
    QWidget *windowMid;
    QWidget *windowRight;

    QPushButton *userAvatar;
    QPushButton *sessionTabBtn;
    QPushButton *friendTabBtn;
    QPushButton *applyTabBtn;

    QLineEdit* searchEdit;
    QPushButton* addFriendBtn;

    SessionFriendArea *sessionFriendArea;

    QLabel *sessionTitleLabel;
    QPushButton *extraBtn;

    MessageShowArea *messageShowArea;
    MessageEditArea *messageEditArea;

    enum class ActiveTab
    {
        SESSION_LIST,
        FRIEND_LIST,
        APPLY_LIST
    };

    ActiveTab activeTab = ActiveTab::SESSION_LIST;
};
#endif // MAINWIDGET_H
