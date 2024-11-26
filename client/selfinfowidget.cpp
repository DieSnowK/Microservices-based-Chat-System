#include "selfinfowidget.h"
#include "debug.hpp"
#include "model/datacenter.h"
#include "toast.h"

using model::DataCenter;
using model::UserInfo;

SelfInfoWidget::SelfInfoWidget(QWidget* parent)
    : QDialog(parent)
{
    this->setFixedSize(500, 250);
    this->setWindowTitle("Personal Information");
    this->setWindowIcon(QIcon(":/resource/image/logo.png"));
    // When the window is closed, the dialog object is automatically destroyed
    this->setAttribute(Qt::WA_DeleteOnClose);
    // Move the window to the current position of the mouse
    this->move(QCursor::pos());

    layout = new QGridLayout();
    layout->setHorizontalSpacing(10);
    layout->setVerticalSpacing(3);
    layout->setContentsMargins(20, 20, 20, 0);
    layout->setAlignment(Qt::AlignTop);
    this->setLayout(layout);

    avatarBtn = new QPushButton();
    avatarBtn->setFixedSize(75, 75);
    avatarBtn->setIconSize(QSize(75, 75));
    avatarBtn->setStyleSheet("QPushButton { border: none; background-color: transparent; }");
    layout->addWidget(avatarBtn, 0, 0, 3, 1);

    QString labelStyle = "QLabel { font-size: 14px; font-weight: 800; }";
    QString btnStyle = "QPushButton { border: none; background-color: transparent; }";
    btnStyle += "QPushButton:pressed { background-color: rgb(210, 210, 210); }";
    QString editStyle = "QLineEdit { border: none; border-radius:5px; padding-left:2px; }";

    int width = 70;
    int height = 30;

    idTag = new QLabel();
    idTag->setFixedSize(width, height);
    idTag->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    idTag->setText("Ordinal");
    idTag->setStyleSheet(labelStyle);

    idLabel = new QLabel();
    idLabel->setFixedHeight(height);
    idLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    nameTag = new QLabel();
    nameTag->setFixedSize(width, height);
    nameTag->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    nameTag->setText("Nickname");
    nameTag->setStyleSheet(labelStyle);

    nameLabel = new QLabel();
    nameLabel->setFixedHeight(height);
    nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    nameModifyBtn = new QPushButton();
    nameModifyBtn->setFixedSize(70, 25);
    nameModifyBtn->setIconSize(QSize(20, 20));
    nameModifyBtn->setIcon(QIcon(":/resource/image/modify.png"));
    nameModifyBtn->setStyleSheet(btnStyle);

    nameEdit = new QLineEdit();
    nameEdit->setFixedHeight(height);
    nameEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    nameEdit->setStyleSheet(editStyle);
    nameEdit->hide();

    nameSubmitBtn = new QPushButton();
    nameSubmitBtn->setFixedSize(70, 25);
    nameSubmitBtn->setIconSize(QSize(20, 20));
    nameSubmitBtn->setIcon(QIcon(":/resource/image/submit.png"));
    nameSubmitBtn->setStyleSheet(btnStyle);
    nameSubmitBtn->hide();

    descTag = new QLabel();
    descTag->setFixedSize(width, height);
    descTag->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    descTag->setText("Desc");
    descTag->setStyleSheet(labelStyle);

    descLabel = new QLabel();
    descLabel->setFixedHeight(height);
    descLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    descModifyBtn = new QPushButton();
    descModifyBtn->setFixedSize(70, 25);
    descModifyBtn->setIconSize(QSize(20, 20));
    descModifyBtn->setIcon(QIcon(":/resource/image/modify.png"));
    descModifyBtn->setStyleSheet(btnStyle);

    descEdit = new QLineEdit();
    descEdit->setFixedHeight(height);
    descEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    descEdit->setStyleSheet(editStyle);
    descEdit->hide();

    descSubmitBtn = new QPushButton();
    descSubmitBtn->setFixedSize(70, 25);
    descSubmitBtn->setIconSize(QSize(20, 20));
    descSubmitBtn->setIcon(QIcon(":/resource/image/submit.png"));
    descSubmitBtn->setStyleSheet(btnStyle);
    descSubmitBtn->hide();

    phoneTag = new QLabel();
    phoneTag->setFixedSize(width, height);
    phoneTag->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    phoneTag->setText("Phone");
    phoneTag->setStyleSheet(labelStyle);

    phoneLabel = new QLabel();
    phoneLabel->setFixedHeight(height);
    phoneLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    phoneModifyBtn = new QPushButton();
    phoneModifyBtn->setFixedSize(70, 25);
    phoneModifyBtn->setIconSize(QSize(20, 20));
    phoneModifyBtn->setIcon(QIcon(":/resource/image/modify.png"));
    phoneModifyBtn->setStyleSheet(btnStyle);

    phoneEdit = new QLineEdit();
    phoneEdit->setFixedHeight(height);
    phoneEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    phoneEdit->setStyleSheet(editStyle);
    phoneEdit->hide();

    phoneSubmitBtn = new QPushButton();
    phoneSubmitBtn->setFixedSize(70, 25);
    phoneSubmitBtn->setIconSize(QSize(20, 20));
    phoneSubmitBtn->setIcon(QIcon(":/resource/image/submit.png"));
    phoneSubmitBtn->setStyleSheet(btnStyle);
    phoneSubmitBtn->hide();

    verifyCodeTag = new QLabel();
    verifyCodeTag->setFixedSize(width, height);
    verifyCodeTag->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    verifyCodeTag->setText("Captcha");
    verifyCodeTag->setStyleSheet(labelStyle);
    verifyCodeTag->hide();

    verifyCodeEdit = new QLineEdit();
    verifyCodeEdit->setFixedHeight(height);
    verifyCodeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    verifyCodeEdit->setStyleSheet(editStyle);
    verifyCodeEdit->hide();

    getVerifyCodeBtn = new QPushButton();
    getVerifyCodeBtn->setText("Get Captcha");
    getVerifyCodeBtn->setStyleSheet("QPushButton { border: none; background-color: transparent; } "
                                    "QPushButton:pressed { background-color: rgb(231, 231, 231); }");
    getVerifyCodeBtn->setFixedSize(70, height);
    getVerifyCodeBtn->hide();

    layout->addWidget(idTag, 0, 1);
    layout->addWidget(idLabel, 0, 2);

    layout->addWidget(nameTag, 1, 1);
    layout->addWidget(nameLabel, 1, 2);
    layout->addWidget(nameModifyBtn, 1, 3);

    layout->addWidget(descTag, 2, 1);
    layout->addWidget(descLabel, 2, 2);
    layout->addWidget(descModifyBtn, 2, 3);

    layout->addWidget(phoneTag, 3, 1);
    layout->addWidget(phoneLabel, 3, 2);
    layout->addWidget(phoneModifyBtn, 3, 3);

#if TEST_UI
    idLabel->setText("1234");
    nameLabel->setText("SnowK");
    descLabel->setText("Cool Boy~");
    phoneLabel->setText("18351958129");
    avatarBtn->setIcon(QIcon(":/resource/image/defaultAvatar.png"));
#endif

    InitSignalSlot();

    DataCenter* dataCenter = DataCenter::GetInstance();
    UserInfo* myself = dataCenter->GetMyself();
    if (myself != nullptr)
    {
        avatarBtn->setIcon(myself->avatar);
        idLabel->setText(myself->userId);
        nameLabel->setText(myself->nickname);
        descLabel->setText(myself->description);
        phoneLabel->setText(myself->phone);
    }
}

void SelfInfoWidget::InitSignalSlot()
{
    connect(nameModifyBtn, &QPushButton::clicked, this, [=]()
    {
        nameLabel->hide();
        nameModifyBtn->hide();
        layout->removeWidget(nameLabel);
        layout->removeWidget(nameModifyBtn);

        nameEdit->show();
        nameSubmitBtn->show();
        layout->addWidget(nameEdit, 1, 2);
        layout->addWidget(nameSubmitBtn, 1, 3);

        nameEdit->setText(nameLabel->text());
    });

    connect(descModifyBtn, &QPushButton::clicked, this, [=]()
    {
        descLabel->hide();
        descModifyBtn->hide();
        layout->removeWidget(descLabel);
        layout->removeWidget(descModifyBtn);

        descEdit->show();
        descSubmitBtn->show();
        layout->addWidget(descEdit, 2, 2);
        layout->addWidget(descSubmitBtn, 2, 3);

        descEdit->setText(descLabel->text());
    });

    connect(phoneModifyBtn, &QPushButton::clicked, this, [=]()
    {
        phoneLabel->hide();
        phoneModifyBtn->hide();
        layout->removeWidget(phoneLabel);
        layout->removeWidget(phoneModifyBtn);

        phoneEdit->show();
        phoneSubmitBtn->show();
        layout->addWidget(phoneEdit, 3, 2);
        layout->addWidget(phoneSubmitBtn, 3, 3);

        verifyCodeTag->show();
        verifyCodeEdit->show();
        getVerifyCodeBtn->show();
        layout->addWidget(verifyCodeTag, 4, 1);
        layout->addWidget(verifyCodeEdit, 4, 2);
        layout->addWidget(getVerifyCodeBtn, 4, 3);

        phoneEdit->setText(phoneLabel->text());
    });

    connect(avatarBtn, &QPushButton::clicked, this, &SelfInfoWidget::ClickAvatarBtn);
    connect(nameSubmitBtn, &QPushButton::clicked, this, &SelfInfoWidget::ClickNameSubmitBtn);
    connect(descSubmitBtn, &QPushButton::clicked, this, &SelfInfoWidget::ClickDescSubmitBtn);
    connect(phoneSubmitBtn, &QPushButton::clicked, this, &SelfInfoWidget::ClickPhoneSubmitBtn);
    connect(getVerifyCodeBtn, &QPushButton::clicked, this, &SelfInfoWidget::ClickGetVerifyCodeBtn);
}

void SelfInfoWidget::ClickNameSubmitBtn()
{
    const QString& nickname = nameEdit->text();
    if (nickname.isEmpty()) {
        return;
    }

    DataCenter* dataCenter = DataCenter::GetInstance();
    connect(dataCenter, &DataCenter::ChangeNicknameDone, this,
            &SelfInfoWidget::ClickNameSubmitBtnDone, Qt::UniqueConnection);
    dataCenter->ChangeNicknameAsync(nickname);
}

void SelfInfoWidget::ClickNameSubmitBtnDone()
{
    layout->removeWidget(nameEdit);
    nameEdit->hide();
    layout->addWidget(nameLabel, 1, 2);
    nameLabel->show();
    nameLabel->setText(nameEdit->text());

    layout->removeWidget(nameSubmitBtn);
    nameSubmitBtn->hide();
    layout->addWidget(nameModifyBtn, 1, 3);
    nameModifyBtn->show();
}

void SelfInfoWidget::ClickDescSubmitBtn()
{
    const QString& desc = descEdit->text();
    if (desc.isEmpty())
    {
        return;
    }

    DataCenter* dataCenter = DataCenter::GetInstance();
    connect(dataCenter, &DataCenter::ChangeDescriptionDone, this,
            &SelfInfoWidget::ChickDescSubmitBtnDone, Qt::UniqueConnection);
    dataCenter->ChangeDescriptionAsync(desc);
}

void SelfInfoWidget::ChickDescSubmitBtnDone()
{
    layout->removeWidget(descEdit);
    descEdit->hide();
    layout->addWidget(descLabel, 2, 2);
    descLabel->show();
    descLabel->setText(descEdit->text());

    layout->removeWidget(descSubmitBtn);
    descSubmitBtn->hide();
    layout->addWidget(descModifyBtn, 2, 3);
    descModifyBtn->show();
}

void SelfInfoWidget::ClickGetVerifyCodeBtn()
{
    const QString& phone = phoneEdit->text();
    if (phone.isEmpty())
    {
        return;
    }

    DataCenter* dataCenter = DataCenter::GetInstance();
    connect(dataCenter, &DataCenter::GetVerifyCodeDone, this, [=]()
    {
        Toast::ShowMessage("SMS verification code has been sent");
    });
    dataCenter->GetVerifyCodeAsync(phone);

    // Save the mobile phone number you just sent the request to
    // Subsequently, click the submit button to modify the phone number. The modified
        // number will not be read from the input box, but this variable will be read
    this->phoneToChange = phone;

    // Disable the send verification code button and give a countdown
    this->getVerifyCodeBtn->setEnabled(false);

    leftTime = 30;
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]()
    {
        if (leftTime <= 1)
        {
            getVerifyCodeBtn->setEnabled(true);
            getVerifyCodeBtn->setText("Get Captcha");
            timer->stop();
            timer->deleteLater();
            return;
        }

        --leftTime;
        getVerifyCodeBtn->setText(QString::number(leftTime) + "s");
    });
    timer->start(1000);
}

void SelfInfoWidget::ClickPhoneSubmitBtn()
{

}

void SelfInfoWidget::ClickPhoneSubmitBtnDone()
{

}

void SelfInfoWidget::ClickAvatarBtn()
{

}

void SelfInfoWidget::ClickAvatarBtnDone()
{

}
