#include "loginwidget.h"
#include "phoneloginwidget.h"
#include "debug.hpp"
#include "toast.h"
#include "model/datacenter.h"
#include "mainwidget.h"

using model::DataCenter;

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(400, 350);
    this->setWindowTitle("Login");
    this->setWindowIcon(QIcon(":/resource/image/logo.png"));
    this->setStyleSheet("QWidget { background-color: rgb(255, 255, 255); }");
    this->setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(50, 0, 50, 0);
    this->setLayout(layout);

    titleLabel = new QLabel();
    titleLabel->setText("Login");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedHeight(50);
    titleLabel->setStyleSheet("QLabel { font-size: 40px; font-weight: 600; }");

    QString editStyle = "QLineEdit { border: none; border-radius: 10px; font-size: 20px; "
                        "padding-left:5px; background-color: rgb(240, 240, 240); }";
    usernameEdit = new QLineEdit();
    usernameEdit->setFixedHeight(40);
    usernameEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    usernameEdit->setPlaceholderText("Enter your username");
    usernameEdit->setStyleSheet(editStyle);

    passwordEdit = new QLineEdit();
    passwordEdit->setFixedHeight(40);
    passwordEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    passwordEdit->setPlaceholderText("Enter your passwoed");
    passwordEdit->setStyleSheet(editStyle);
    passwordEdit->setEchoMode(QLineEdit::Password);

    verifyCodeEdit = new QLineEdit();
    verifyCodeEdit->setFixedHeight(40);
    verifyCodeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    verifyCodeEdit->setPlaceholderText("Enter captcha");
    verifyCodeEdit->setStyleSheet(editStyle);

    verifyCodeWidget = new VerifyCodeWidget(this);

    submitBtn = new QPushButton();
    submitBtn->setText("Login");
    submitBtn->setFixedHeight(40);
    submitBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QString btnGreenStyle = "QPushButton { border: none; border-radius: 10px; "
                            "background-color: rgb(44, 182, 61); color: rgb(255, 255, 255); }";
    btnGreenStyle += "QPushButton:pressed { background-color: rgb(240, 240, 240); }";
    submitBtn->setStyleSheet(btnGreenStyle);

    phoneModeBtn = new QPushButton();
    phoneModeBtn->setFixedSize(100, 40);
    phoneModeBtn->setText("Phone-Login");
    QString btnWhiteStyle = "QPushButton { border: none; border-radius: 10px; "
                            "background-color: transparent; }";
    btnWhiteStyle += "QPushButton:pressed { background-color: rgb(240, 240, 240); }";
    phoneModeBtn->setStyleSheet(btnWhiteStyle);

    switchModeBtn = new QPushButton();
    switchModeBtn->setFixedSize(100, 40);
    switchModeBtn->setText("Registry");
    switchModeBtn->setStyleSheet(btnWhiteStyle);

    layout->addWidget(titleLabel, 0, 0, 1, 5);
    layout->addWidget(usernameEdit, 1, 0, 1, 5);
    layout->addWidget(passwordEdit, 2, 0, 1, 5);
    layout->addWidget(verifyCodeEdit, 3, 0, 1, 4);
    layout->addWidget(verifyCodeWidget, 3, 4, 1, 1);
    layout->addWidget(submitBtn, 4, 0, 1, 5);
    layout->addWidget(phoneModeBtn, 5, 0, 1, 1);
    layout->addWidget(switchModeBtn, 5, 4, 1, 1);

    connect(switchModeBtn, &QPushButton::clicked, this, &LoginWidget::SwitchMode);

    // TODO
    connect(phoneModeBtn, &QPushButton::clicked, this, [=]()
    {
        PhoneLoginWidget* phoneLoginWidget = new PhoneLoginWidget(nullptr);
        phoneLoginWidget->show();

        this->close();
    });

#if TEST_TOAST
    connect(submitBtn, &QPushButton::clicked, this, [=]()
    {
        Toast::ShowMessage("Welcome to SnowK's home~");
    });
#else
    connect(submitBtn, &QPushButton::clicked, this, &LoginWidget::ClickSubmitBtn);
#endif
}

void LoginWidget::SwitchMode()
{
    if (isLoginMode)
    {
        this->setWindowTitle("Registry");
        titleLabel->setText("Registry");
        submitBtn->setText("Registry");
        phoneModeBtn->setText("Phone-Registry");
        switchModeBtn->setText("Login");
    }
    else
    {
        this->setWindowTitle("Login");
        titleLabel->setText("Login");
        submitBtn->setText("Login");
        phoneModeBtn->setText("Phone-Login");
        switchModeBtn->setText("Registry");
    }

    isLoginMode = !isLoginMode;
}

void LoginWidget::ClickSubmitBtn()
{
    const QString& username = usernameEdit->text();
    const QString& password = passwordEdit->text();
    const QString& verifyCode = verifyCodeEdit->text();
    if (username.isEmpty() || password.isEmpty() || verifyCode.isEmpty())
    {
        Toast::ShowMessage("Username/password/verification code cannot be empty");
        return;
    }

    if (!verifyCodeWidget->CheckVerifyCode(verifyCode))
    {
        verifyCodeWidget->RefreshVerifyCode();
        Toast::ShowMessage("Verification code is incorrect");
        return;
    }

    DataCenter* dataCenter = DataCenter::GetInstance();
    if (isLoginMode)
    {
        connect(dataCenter, &DataCenter::UserLoginDone, this, &LoginWidget::UserLoginDone);
        dataCenter->UserLoginAsync(username, password);
    }
    else
    {
        connect(dataCenter, &DataCenter::UserRegisterDone, this, &LoginWidget::UserRegisterDone);
        dataCenter->UserRegisterAsync(username, password);
    }
}

void LoginWidget::UserLoginDone(bool ok, const QString &reason)
{
    if (!ok)
    {
        Toast::ShowMessage("Login failed" + reason);
        return;
    }

    MainWidget* mainWidget = MainWidget::GetInstance();
    mainWidget->show();

    this->close();
}

void LoginWidget::UserRegisterDone(bool ok, const QString &reason)
{

}
