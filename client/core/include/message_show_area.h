#ifndef MESSAGESHOWAREA_H
#define MESSAGESHOWAREA_H

#include <QScrollArea>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include "data.hpp"

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
    void ScrollToEnd();

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
/// -> MessageType::SPEECH_TYPE
////////////////////////////////////////////////////////

class MessageContentLabel : public QWidget
{
    Q_OBJECT

public:
    MessageContentLabel(const QString& text, bool isLeft, MessageType msgType,
                        const QString& fileId, const QByteArray& content);

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void UpdateUI(const QString& fileId, const QByteArray& fileContent);
    void SaveAsFile(const QByteArray& content);

    void PlayDone();

    void contextMenuEvent(QContextMenuEvent* event) override;
    void SpeechConvertTextDone(const QString& fileId, const QString& text);

private:
    bool isLeft;
    QLabel* label;

    model::MessageType msgType;
    QString fileId;
    QByteArray content;

    bool loadContentDone = false;
};

////////////////////////////////////////////////////////
/// MessageImageLabel
////////////////////////////////////////////////////////
class MessageImageLabel : public QWidget
{
    Q_OBJECT

public:
    MessageImageLabel(const QString& fileId, const QByteArray& content, bool isLeft);

    void UpdateUI(const QString& fileId, const QByteArray& content);
    void paintEvent(QPaintEvent* event);

private:
    QPushButton* imageBtn;
    QString fileId;
    QByteArray content;
    bool isLeft;
};

#endif // MESSAGESHOWAREA_H
