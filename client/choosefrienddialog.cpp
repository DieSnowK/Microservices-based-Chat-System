#include "choosefrienddialog.h"

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

    // InitLeft(layout);
    // InitRight(layout);

    // InitData();
}

void ChooseFriendDialog::InitLeft(QHBoxLayout *layout)
{
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidgetResizable(true);
    scrollArea->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal { height: 0px;}");
    scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 2px; "
                                                   "background-color: rgb(255, 255, 255) }");
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
    for (int i = 0; i < 30; ++i) {
        this->addFriend(QString::number(1000 + i), defaultAvatar, "张三" + QString::number(i), false);
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
    tipLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: 700}");

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 2px; "
                                                   "background-color: rgb(255, 255, 255);}");
    scrollArea->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {height: 0px;}");
    scrollArea->setStyleSheet("QScrollArea {border: none;}");
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
                    "border: none; border-radius: 5px;}";
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
    cancelBtn->setText("Cancle");
    cancelBtn->setStyleSheet(style);

    gridLayout->addWidget(tipLabel, 0, 0, 1, 9);
    gridLayout->addWidget(scrollArea, 1, 0, 1, 9);
    gridLayout->addWidget(okBtn, 2, 1, 1, 3);
    gridLayout->addWidget(cancelBtn, 2, 5, 1, 3);

#if 0
    QIcon defaultAvatar(":/resource/image/defaultAvatar.png");
    for (int i = 0; i < 10; ++i) {
        this->addSelectedFriend(QString::number(1000 + i), defaultAvatar, "张三" + QString::number(i));
    }
#endif

    // connect(okBtn, &QPushButton::clicked, this, &ChooseFriendDialog::clickOkBtn);
    // connect(cancelBtn, &QPushButton::clicked, this, [=]() {
    //     this->close();
    // });
}
