#ifndef PHONELOGINWIDGET_H
#define PHONELOGINWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QGridLayout>

class PhoneLoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PhoneLoginWidget(QWidget *parent = nullptr);

    void InitSignalSlot();
    void SwitchMode();

    void SendVerifyCode();
    void SendVerifyCodeDone();

    void ClickSubmitBtn();
    void PhoneLoginDone(bool ok, const QString& reason);
    void PhoneRegisterDone(bool ok, const QString& reason);

    void CountDown();

private:
    QLineEdit* phoneEdit;
    QPushButton* sendVerifyCodeBtn;
    QLineEdit* verifyCodeEdit;
    QLabel* titleLabel;
    QPushButton* submitBtn;
    QPushButton* userModeBtn;
    QPushButton* switchModeBtn;

    bool isLoginMode = true;
    QString currentPhone = "";  // Record which phone number was used to send the verification code
    QTimer* timer;
    int leftTime = 30;

signals:
};

#endif // PHONELOGINWIDGET_H
