#ifndef SESSIONFRIENDAREA_H
#define SESSIONFRIENDAREA_H

#include <QWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStyleOption>
#include <QPainter>


//////////////////////////////////////////////////////////
/// SessionFriendArea
//////////////////////////////////////////////////////////

enum class ItemType
{
    SESSION_ITEM_TYPE,
    FRIEND_ITEM_TYPE,
    APPLY_ITEM_TYPE
};

//////////////////////////////////////////////////////////
/// SessionFriendArea
//////////////////////////////////////////////////////////

class SessionFriendArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit SessionFriendArea(QWidget *parent = nullptr);

    void Clear();
    void AddItem(ItemType itemtype, const QString& id, const QIcon& avatar,
                 const QString& name, const QString& text);

    // Select a specific item and select it by the index subscript
    void ClickItem(int index);

private:
    // Adding elements to the layout inside the container
        // will trigger the QScrollArea scrolling effect
    QWidget* container;

signals:
};

//////////////////////////////////////////////////////////
/// SessionFriendItem
//////////////////////////////////////////////////////////

class SessionFriendItem : public QWidget
{
    Q_OBJECT
public:
    SessionFriendItem(QWidget* owner, const QIcon& avatar,
                      const QString& name, const QString& text);

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

    void Select();
    virtual void Active();

private:
    // owner points to the SessionFriendArea above
    QWidget* owner;

    // This variable is used to indicate whether the current item is "selected" or not
    bool selected = false;

protected:
    QLabel* messageLabel;
};

//////////////////////////////////////////////////////////
/// SessionItem
//////////////////////////////////////////////////////////

class SessionItem : public SessionFriendItem
{
    Q_OBJECT

public:
    SessionItem(QWidget* owner, const QString& chatSessionId,
                const QIcon& avatar, const QString& name, const QString& lastMessage);

    void Active() override;
    void UpdateLastMessage(const QString& chatSessionId);

private:
    QString chatSessionId;
    QString text; // Text preview of the last message
};

//////////////////////////////////////////////////////////
/// FriendItem
//////////////////////////////////////////////////////////

class FriendItem : public SessionFriendItem
{
    Q_OBJECT

public:
    FriendItem(QWidget* owner, const QString& userId, const QIcon& avatar,
               const QString& name, const QString& description);

    void Active() override;

private:
    QString userId;
};

//////////////////////////////////////////////////////////
/// ApplyItem
//////////////////////////////////////////////////////////

class ApplyItem : public SessionFriendItem
{
    Q_OBJECT

public:
    ApplyItem(QWidget* owner, const QString& userId,
              const QIcon& avatar, const QString& name);

    void Active() override;
    
private:
    QString userId;
};

#endif // SESSIONFRIENDAREA_H
