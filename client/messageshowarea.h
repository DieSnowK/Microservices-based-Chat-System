#ifndef MESSAGESHOWAREA_H
#define MESSAGESHOWAREA_H

#include <QScrollArea>
#include <QWidget>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>

#include "model/data.hpp"
#include "userinfowidget.h"

using model::Message;
using model::MessageType;
using model::UserInfo;

////////////////////////////////////////////////////////
/// MessageShowArea
////////////////////////////////////////////////////////

class MessageShowArea : public QScrollArea
{
    Q_OBJECT
public:
    MessageShowArea();
    
    void AddMessage(bool isLeft, const Message& message);
    void AddFrontMessage(bool isLeft, const Message& message);
    void Clear();

private:
    QWidget *container;
};

////////////////////////////////////////////////////////
/// MessageItem
////////////////////////////////////////////////////////

class MessageItem : public QWidget
{
    Q_OBJECT

public:
    MessageItem(bool isLeft);

    // Factory Pattern
    static MessageItem* MakeMessageItem(bool isLeft, const Message& message);
    
    static QWidget* MakeTextMessageItem(bool isLeft, const QString& text);
    static QWidget* MakeImageMessageItem(bool isLeft, const QString& fileId, const QByteArray& content);
    static QWidget* MakeFileMessageItem(bool isLeft, const Message& message);
    static QWidget* MakeSpeechMessageItem(bool isLeft, const Message& message);

private:
    bool isLeft;
};

////////////////////////////////////////////////////////
/// MessageContentLabel
/// -> MessageType::TEXT_TYPE && MessageType::FILE_TYPE
////////////////////////////////////////////////////////

class MessageContentLabel : public QWidget
{
    Q_OBJECT

public:
    MessageContentLabel(const QString& text, bool isLeft, MessageType msgType,
                        const QString& fileId, const QByteArray& content);

    void paintEvent(QPaintEvent* event) override;
    // void mousePressEvent(QMouseEvent* event) override;

private:
    bool isLeft;
    QLabel* label;

    model::MessageType msgType;
    QString fileId;
    QByteArray content;

    bool loadContentDone = false;
};

#endif // MESSAGESHOWAREA_H
