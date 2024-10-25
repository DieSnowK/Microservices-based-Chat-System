#ifndef MESSAGESHOWAREA_H
#define MESSAGESHOWAREA_H

#include <QScrollArea>
#include <QWidget>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

// using model::Message;

class MessageShowArea : public QScrollArea
{
    Q_OBJECT
public:
    MessageShowArea();
    
    void AddMessage(bool isLeft, const Message& message);
    void AddFrontMessage(bool isLeft, const Message& message);

private:
    QWidget *container;
};

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

#endif // MESSAGESHOWAREA_H
