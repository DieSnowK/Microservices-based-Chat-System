#ifndef CHOOSEFRIENDDIALOG_H
#define CHOOSEFRIENDDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QPainter>

class ChooseFriendDialog;

////////////////////////////////////////////////
/// ChooseFriendItem
////////////////////////////////////////////////

class ChooseFriendItem : public QWidget
{
    Q_OBJECT

public:
    ChooseFriendItem(ChooseFriendDialog* owner, const QString& userId,
                     const QIcon& avatar, const QString& name, bool checked);

    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

    const QString& GetUserId() const;
    QCheckBox* GetCheckBox();

private:
    bool isHover = false;

    QCheckBox* checkBox;
    QPushButton* avatarBtn;
    QLabel* nameLabel;
    ChooseFriendDialog* owner;

    QString userId;
};

////////////////////////////////////////////////
/// ChooseFriendDialog
////////////////////////////////////////////////

class ChooseFriendDialog : public QDialog
{
    Q_OBJECT

public:
    ChooseFriendDialog(QWidget* parent, const QString& userId);

    void InitLeft(QHBoxLayout* layout);
    void InitRight(QHBoxLayout* layout);
    void InitData();

    void ClickOkBtn();
    QList<QString> GenerateMemberList();

    void AddFriend(const QString& userId, const QIcon& avatar, const QString& name, bool checked);
    void AddSelectedFriend(const QString& userId, const QIcon& avatar, const QString& name);
    void DeleteSelectedFriend(const QString& userId);

private:
    QWidget* totalContainer;
    QWidget* selectedContainer;
    QString currentUserId;
};

#endif // CHOOSEFRIENDDIALOG_H
