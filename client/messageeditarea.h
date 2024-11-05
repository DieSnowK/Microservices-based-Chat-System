#ifndef MESSAGEEDITAREA_H
#define MESSAGEEDITAREA_H

#include <QWidget>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>

#include "historymessagewidget.h"

class MessageEditArea : public QWidget
{
    Q_OBJECT

public:
    explicit MessageEditArea(QWidget *parent = nullptr);
    void InitSignalSlot();


private:
    QPushButton* sendImageBtn;
    QPushButton* sendFileBtn;
    QPushButton* sendSpeechBtn;
    QPushButton* showHistoryBtn;
    QPlainTextEdit* textEdit;
    QPushButton* sendTextBtn;
    QLabel* tipLabel;

signals:
};

#endif // MESSAGEEDITAREA_H
