#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui 
{
    class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

private:
    static MainWidget* instance;
    MainWidget(QWidget *parent = nullptr);

public:
    ~MainWidget();
    static MainWidget* GetInstance();

private:
    Ui::MainWidget *ui;

    QWidget* windowLeft;
    QWidget* windowMid;
    QWidget* windowRight;

    QPushButton* userAvatar;
    QPushButton* sessionTabBtn;
    QPushButton* friendTabBtn;
    QPushButton* applyTabBtn;

public:
    void InitMainWindow();
    void InitLeftWindow();
    void InitMidWindow();
    void InitRightWindow();
};
#endif // MAINWIDGET_H
