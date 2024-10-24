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

class SessionFriendArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit SessionFriendArea(QWidget *parent = nullptr);

    void Clear();
    void AddItem(const QIcon& avatar, const QString& name, const QString& text);

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

private:
    // owner points to the SessionFriendArea above
    QWidget* owner;

    // This variable is used to indicate whether the current item is "selected" or not
    bool selected = false;
};

#endif // SESSIONFRIENDAREA_H
