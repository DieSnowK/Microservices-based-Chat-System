#ifndef HISTORYMESSAGEWIDGET_H
#define HISTORYMESSAGEWIDGET_H

#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QRadioButton>
#include <QDateTimeEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>

#include "model/data.hpp"

using model::Message;
using model::UserInfo;
using model::MessageType;

////////////////////////////////////////////////////////////////////
/// HistoryItem
////////////////////////////////////////////////////////////////////

class HistoryItem : public QWidget
{
    Q_OBJECT

public:
    HistoryItem() {}

    static HistoryItem* MakeHistoryItem(const Message& message);
};


////////////////////////////////////////////////////////////////////
/// HistoryMessageWidget
////////////////////////////////////////////////////////////////////

class HistoryMessageWidget : public QDialog
{
    Q_OBJECT

public:
    HistoryMessageWidget(QWidget* parent);

    void AddHistoryMessage(const Message& message);
    void Clear();

    void ClickSearchBtn();
    void ClickSearchBtnDone();

private:
    void InitScrollArea(QGridLayout* layout);

private:
    QWidget* container;
    QLineEdit* searchEdit;
    QRadioButton* keyRadioBtn;
    QRadioButton* timeRadioBtn;
    QDateTimeEdit* begTimeEdit;
    QDateTimeEdit* endTimeEdit;
};

////////////////////////////////////////////////////////////////////
/// ImageButton
////////////////////////////////////////////////////////////////////

class ImageButton : public QPushButton
{
public:
    ImageButton(const QString& fileId, const QByteArray& content);
    void UpdateUI(const QString& fileId, const QByteArray& content);

private:
    QString fileId;
};

////////////////////////////////////////////////////////////////////
/// FileLabel
////////////////////////////////////////////////////////////////////

class FileLabel : public QLabel
{
public:
    FileLabel(const QString& fileId, const QString& fileName);

    void GetContentDone(const QString& fileId, const QByteArray& fileContent);
    void mousePressEvent(QMouseEvent* event) override;

private:
    QString fileId;
    QByteArray content;
    QString fileName;
    bool loadDone = false;
};

////////////////////////////////////////////////////////////////////
/// SpeechLabel
////////////////////////////////////////////////////////////////////

class SpeechLabel : public QLabel
{
public:
    SpeechLabel(const QString& fileId);

    void GetContentDone(const QString& fileId, const QByteArray& content);
    void mousePressEvent(QMouseEvent* event) override;

private:
    QString fileId;
    QByteArray content;
    bool loadDone = false;
};

#endif // HISTORYMESSAGEWIDGET_H
