#include "phoneloginwidget.h"
#include "loginwidget.h"
#include "model/datacenter.h"
#include "toast.h"

using model::DataCenter;

PhoneLoginWidget::PhoneLoginWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(400, 350);
    this->setWindowTitle("Login");
    this->setWindowIcon(QIcon(":/resource/image/logo.png"));
    this->setStyleSheet("QWidget { background-color: rgb(255, 255, 255); }");
    this->setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(10);
    layout->setContentsMargins(50, 0, 50, 0);
    this->setLayout(layout);

    titleLabel = new QLabel();
    titleLabel->setText("Login");
    titleLabel->setFixedHeight(50);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    titleLabel->setStyleSheet("QLabel { font-size: 40px; font-weight: 600; }");
    titleLabel->setAlignment(Qt::AlignCenter);

    QString editStyle = "QLineEdit { border: none; background-color: rgb(240, 240, 240); "
                        "font-size: 20px; border-radius: 10px; padding-left: 5px;}";
    phoneEdit = new QLineEdit();
    phoneEdit->setPlaceholderText("Enter your phone number");
    phoneEdit->setFixedHeight(40);
    phoneEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    phoneEdit->setStyleSheet(editStyle);

    verifyCodeEdit = new QLineEdit();
    verifyCodeEdit->setPlaceholderText("Enter the SMS verification code");
    verifyCodeEdit->setFixedHeight(40);
    verifyCodeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    verifyCodeEdit->setStyleSheet(editStyle);

    QString btnWhiteStyle = "QPushButton { border: none; border-radius: 10px; "
                            "background-color: transparent; }";
    btnWhiteStyle += "QPushButton:pressed { background-color: rgb(240, 240, 240); }";
    sendVerifyCodeBtn = new QPushButton();
    sendVerifyCodeBtn->setFixedSize(100, 40);
    sendVerifyCodeBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sendVerifyCodeBtn->setText("Send Captcha");
    sendVerifyCodeBtn->setStyleSheet(btnWhiteStyle);

    submitBtn = new QPushButton();
    submitBtn->setFixedHeight(40);
    submitBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    submitBtn->setText("Login");
    QString btnGreenStyle = "QPushButton { border: none; border-radius: 10px; "
                            "background-color: rgb(44, 182, 61); color: rgb(255, 255, 255); }";
    btnGreenStyle += "QPushButton:pressed { background-color: rgb(240, 240, 240); }";
    submitBtn->setStyleSheet(btnGreenStyle);

    userModeBtn = new QPushButton();
    userModeBtn->setFixedSize(100, 40);
    userModeBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    userModeBtn->setText("User-Login");
    userModeBtn->setStyleSheet(btnWhiteStyle);

    switchModeBtn = new QPushButton();
    switchModeBtn->setFixedSize(100, 40);
    switchModeBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    switchModeBtn->setText("Registry");
    switchModeBtn->setStyleSheet(btnWhiteStyle);

    layout->addWidget(titleLabel, 0, 0, 1, 5);
    layout->addWidget(phoneEdit, 1, 0, 1, 5);
    layout->addWidget(verifyCodeEdit, 2, 0, 1, 4);
    layout->addWidget(sendVerifyCodeBtn, 2, 4, 1, 1);
    layout->addWidget(submitBtn, 3, 0, 1, 5);
    layout->addWidget(userModeBtn, 4, 0, 1, 1);
    layout->addWidget(switchModeBtn, 4, 4, 1, 1);

    InitSignalSlot();
}

void PhoneLoginWidget::InitSignalSlot()
{
    connect(switchModeBtn, &QPushButton::clicked, this, &PhoneLoginWidget::SwitchMode);

    connect(userModeBtn, &QPushButton::clicked, this, [=]()
    {
        LoginWidget* loginWidget = new LoginWidget(nullptr);
        loginWidget->show();
        this->close();
    });

    connect(sendVerifyCodeBtn, &QPushButton::clicked, this, &PhoneLoginWidget::SendVerifyCode);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PhoneLoginWidget::CountDown);

    connect(submitBtn, &QPushButton::clicked, this, &PhoneLoginWidget::ClickSubmitBtn);
}

void PhoneLoginWidget::SwitchMode()
{
    if (isLoginMode)
    {
        this->setWindowTitle("Registry");
        titleLabel->setText("Registry");
        submitBtn->setText("Registry");
        userModeBtn->setText("User-Registry");
        switchModeBtn->setText("Login");
    }
    else
    {
        this->setWindowTitle("Login");
        titleLabel->setText("Login");
        submitBtn->setText("Login");
        userModeBtn->setText("User-Login");
        switchModeBtn->setText("Registry");
    }

    isLoginMode = !isLoginMode;
}

void PhoneLoginWidget::SendVerifyCode()
{
    const QString& phone = this->phoneEdit->text();
    if (phone.isEmpty())
    {
        return;
    }
    this->currentPhone = phone;

    DataCenter* dataCenter = DataCenter::GetInstance();
    connect(dataCenter, &DataCenter::GetVerifyCodeDone, this,
            &PhoneLoginWidget::SendVerifyCodeDone, Qt::UniqueConnection);
    dataCenter->GetVerifyCodeAsync(phone);

    timer->start(1000);
}

void PhoneLoginWidget::SendVerifyCodeDone()
{
    Toast::ShowMessage("A verification code has been sent");
}

void PhoneLoginWidget::ClickSubmitBtn()
{

}

void PhoneLoginWidget::PhoneLoginDone(bool ok, const QString &reason)
{

}

void PhoneLoginWidget::PhoneRegisterDone(bool ok, const QString &reason)
{

}

void PhoneLoginWidget::CountDown()
{
    if (leftTime <= 1)
    {
        sendVerifyCodeBtn->setEnabled(true);
        sendVerifyCodeBtn->setText("Send Captcha");

        timer->stop();
        leftTime = 30;

        return;
    }

    leftTime -= 1;
    sendVerifyCodeBtn->setText(QString::number(leftTime) + " s");
    if (sendVerifyCodeBtn->isEnabled())
    {
        sendVerifyCodeBtn->setEnabled(false);
    }
}
