#include "choosefrienddialog.h"
#include "debug.hpp"

////////////////////////////////////////////////
/// ChooseFriendItem
////////////////////////////////////////////////

ChooseFriendItem::ChooseFriendItem(ChooseFriendDialog *owner, const QString &userId,
                                   const QIcon &avatar, const QString &name, bool checked)
    : userId(userId)
{
    this->setFixedHeight(50);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setSpacing(10);
    layout->setContentsMargins(20, 0, 20, 0);
    this->setLayout(layout);

    checkBox = new QCheckBox();
    checkBox->setChecked(checked);
    checkBox->setFixedSize(25, 25);
    QString style = "QCheckBox { background-color: transparent; }"
                    "QCheckBox::indicator { width: 20px; height: 20px;"
                    "image: url(:/resource/image/unchecked.png); }";
    style += "QCheckBox::indicator:checked { image: url(:/resource/image/checked.png);}";
    checkBox->setStyleSheet(style);

    avatarBtn = new QPushButton();
    avatarBtn->setFixedSize(40, 40);
    avatarBtn->setIconSize(QSize(40, 40));
    avatarBtn->setIcon(avatar);

    nameLabel = new QLabel();
    nameLabel->setText(name);
    nameLabel->setStyleSheet("QLabel {background-color: transparent;}");

    layout->addWidget(checkBox);
    layout->addWidget(avatarBtn);
    layout->addWidget(nameLabel);

    connect(checkBox, &QCheckBox::toggled, this, [=](bool checked)
    {
        if (checked)
        {
            owner->AddSelectedFriend(userId, avatar, name);
        }
        else
        {
            owner->DeleteSelectedFriend(userId);
        }
    });
}

void ChooseFriendItem::paintEvent(QPaintEvent *event)
{
    (void) event;

    QPainter painter(this);
    if (isHover)
    {
        painter.fillRect(this->rect(), QColor(230, 230, 230));
    }
    else
    {
        painter.fillRect(this->rect(), QColor(255, 255, 255));
    }
}

void ChooseFriendItem::enterEvent(QEnterEvent *event)
{
    (void) event;
    isHover = true;

    // "update()" is equivalent to "update interface"
    this->update(); // ==> this->repaint();
}

void ChooseFriendItem::leaveEvent(QEvent *event)
{
    (void) event;
    isHover =  false;
    this->update();
}

const QString &ChooseFriendItem::GetUserId() const
{
    return userId;
}

QCheckBox *ChooseFriendItem::GetCheckBox()
{
    return checkBox;
}

////////////////////////////////////////////////
/// ChooseFriendDialog
////////////////////////////////////////////////

ChooseFriendDialog::ChooseFriendDialog(QWidget *parent)
{
    this->setWindowTitle("Choose Friend");
    this->setWindowIcon(QIcon(":/resource/image/logo.png"));
    this->setFixedSize(750, 550);
    this->setStyleSheet("QDialog { background-color: rgb(255, 255, 255);}");
    this->setAttribute(Qt::WA_DeleteOnClose);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);

    InitLeft(layout);
    InitRight(layout);

    // InitData();
}

void ChooseFriendDialog::InitLeft(QHBoxLayout *layout)
{
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidgetResizable(true);
    scrollArea->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal { height: 0px; }");
    scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 2px; "
                                                   "background-color: rgb(255, 255, 255); }");
    scrollArea->setStyleSheet("QScrollArea { border:none; }");
    layout->addWidget(scrollArea, 1);

    totalContainer = new QWidget();
    totalContainer->setObjectName("totalContainer");
    totalContainer->setStyleSheet("#totalContainer { background-color: rgb(255, 255, 255); }");
    scrollArea->setWidget(totalContainer);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->setAlignment(Qt::AlignTop);
    totalContainer->setLayout(vlayout);

#if TEST_UI
    QIcon defaultAvatar(":/resource/image/defaultAvatar.png");
    for (int i = 0; i < 30; ++i)
    {
        this->AddFriend(QString::number(1000 + i), defaultAvatar, "SnowK" + QString::number(i), false);
    }
#endif
}

void ChooseFriendDialog::InitRight(QHBoxLayout *layout)
{
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(20, 0, 20, 20);
    gridLayout->setSpacing(10);
    layout->addLayout(gridLayout, 1);

    QLabel* tipLabel = new QLabel();
    tipLabel->setText("Select Contacts");
    tipLabel->setFixedHeight(30);
    tipLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    tipLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    tipLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: 700 }");

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 2px; "
                                                   "background-color: rgb(255, 255, 255); }");
    scrollArea->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal { height: 0px; }");
    scrollArea->setStyleSheet("QScrollArea { border: none; }");
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    selectedContainer = new QWidget();
    selectedContainer->setObjectName("selectedContainer");
    selectedContainer->setStyleSheet("#selectedContainer { background-color: rgb(255, 255, 255); }");
    scrollArea->setWidget(selectedContainer);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->setAlignment(Qt::AlignTop);
    selectedContainer->setLayout(vlayout);

    QString style = "QPushButton { color: rgb(7, 191, 96); "
                    "background-color: rgb(240, 240, 240); "
                    "border: none; border-radius: 5px; }";
    style += "QPushButton:hover { background-color: rgb(220, 220, 220); }"
             "QPushButton:pressed { background-color: rgb(200, 200, 200); }";

    QPushButton* okBtn = new QPushButton();
    okBtn->setFixedHeight(40);
    okBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    okBtn->setText("Finish");
    okBtn->setStyleSheet(style);

    QPushButton* cancelBtn = new QPushButton();
    cancelBtn->setFixedHeight(40);
    cancelBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cancelBtn->setText("Cancel");
    cancelBtn->setStyleSheet(style);

    gridLayout->addWidget(tipLabel, 0, 0, 1, 9);
    gridLayout->addWidget(scrollArea, 1, 0, 1, 9);
    gridLayout->addWidget(okBtn, 2, 1, 1, 3);
    gridLayout->addWidget(cancelBtn, 2, 5, 1, 3);

#if 0
    QIcon defaultAvatar(":/resource/image/defaultAvatar.png");
    for (int i = 0; i < 10; ++i)
    {
        this->AddSelectedFriend(QString::number(1000 + i), defaultAvatar, "SnowK" + QString::number(i));
    }
#endif

    // connect(okBtn, &QPushButton::clicked, this, &ChooseFriendDialog::clickOkBtn);
    // connect(cancelBtn, &QPushButton::clicked, this, [=]()
    // {
    //     this->close();
    // });
}

void ChooseFriendDialog::AddFriend(const QString &userId, const QIcon &avatar, const QString &name, bool checked)
{
    ChooseFriendItem* item = new ChooseFriendItem(this, userId, avatar, name, checked);
    totalContainer->layout()->addWidget(item);
}

void ChooseFriendDialog::AddSelectedFriend(const QString &userId, const QIcon &avatar, const QString &name)
{
    ChooseFriendItem* item = new ChooseFriendItem(this, userId, avatar, name, true);
    selectedContainer->layout()->addWidget(item);
}

void ChooseFriendDialog::DeleteSelectedFriend(const QString &userId)
{
    QVBoxLayout* vlayout = dynamic_cast<QVBoxLayout*>(selectedContainer->layout());
    for (int i = vlayout->count() - 1; i >= 0; --i)
    {
        auto* item = vlayout->itemAt(i);
        if (item == nullptr || item->widget() == nullptr)
        {
            continue;
        }

        ChooseFriendItem* chooseFriendItem = dynamic_cast<ChooseFriendItem*>(item->widget());
        if (chooseFriendItem->GetUserId() != userId)
        {
            continue;
        }
        vlayout->removeWidget(chooseFriendItem);

        // 此处直接使用 delete 可能导致程序直接崩溃. 因为 delete 该对象的时候, 该对象内部的 QCheckBox 还在使用中 (触发着信号槽呢)
    //     // 改成 deleteLater, 就相当于把 delete 操作委托给 Qt 自身来完成了. 告诉 Qt 框架说, 你要删除这个对象. 至于啥时候删除 Qt
    //     // 会确保在 Qt 自身用完了之后, 去真正删除.
    //     // delete chooseFriendItem;
        chooseFriendItem->deleteLater();
    }

    QVBoxLayout* vlayoutLeft = dynamic_cast<QVBoxLayout*>(totalContainer->layout());
    for (int i = 0; i < vlayoutLeft->count(); ++i)
    {
        auto* item = vlayoutLeft->itemAt(i);
        if (item == nullptr || item->widget() == nullptr)
        {
            continue;
        }

        ChooseFriendItem* chooseFriendItem = dynamic_cast<ChooseFriendItem*>(item->widget());
        if (chooseFriendItem->GetUserId() != userId)
        {
            continue;
        }

        chooseFriendItem->GetCheckBox()->setChecked(false);
    }
}
