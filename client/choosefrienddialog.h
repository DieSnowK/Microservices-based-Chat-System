#ifndef CHOOSEFRIENDDIALOG_H
#define CHOOSEFRIENDDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QPushButton>

class ChooseFriendDialog : public QDialog
{
    Q_OBJECT

public:
    ChooseFriendDialog(QWidget *parent);

    void InitLeft(QHBoxLayout* layout);
    void InitRight(QHBoxLayout* layout);

private:
    QWidget* totalContainer;
    QWidget* selectedContainer;
};

#endif // CHOOSEFRIENDDIALOG_H
