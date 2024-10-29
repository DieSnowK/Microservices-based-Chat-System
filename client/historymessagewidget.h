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

#endif // HISTORYMESSAGEWIDGET_H
