#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

#include "verifycodewidget.h"

class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);

    void SwitchMode();

    void ClickSubmitBtn();
    void UserLoginDone(bool ok, const QString& reason);
    void UserRegisterDone(bool ok, const QString& reason);

private:
    bool isLoginMode = true;

    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QLineEdit* verifyCodeEdit;
    VerifyCodeWidget* verifyCodeWidget;

    QLabel* titleLabel;
    QPushButton* submitBtn;
    QPushButton* phoneModeBtn;
    QPushButton* switchModeBtn;
};

#endif // LOGINWIDGET_H
