#include <QScrollArea>
#include <QScrollBar>

#include "group_session_detail_widget.h"
#include "debug.h"
#include "datacenter.h"

using model::DataCenter;

GroupSessionDetailWidget::GroupSessionDetailWidget(QWidget* parent)
    : QDialog(parent)
{
    this->setFixedSize(410, 600);
    this->setWindowTitle("Group Details");
    this->setWindowIcon(QIcon(":/resource/image/logo.png"));
    this->setStyleSheet("QDialog { background-color: rgb(255, 255, 255); }");
    this->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(10);
    vlayout->setContentsMargins(50, 20, 50, 50);
    vlayout->setAlignment(Qt::AlignTop);
    this->setLayout(vlayout);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 2px; "
                                                   "background-color: rgb(255, 255, 255); }");
    scrollArea->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal { height: 0; }");
    scrollArea->setStyleSheet("QWidget { background-color: transparent; border: none; }");
    scrollArea->setFixedSize(310, 350);
    scrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QWidget* container = new QWidget();
    scrollArea->setWidget(container);

    glayout = new QGridLayout();
    glayout->setSpacing(10);
    glayout->setContentsMargins(0, 0, 0, 0);
    glayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    container->setLayout(glayout);

    vlayout->addWidget(scrollArea);

    AvatarItem* addBtn = new AvatarItem(QIcon(":/resource/image/cross.png"), "Add");
    glayout->addWidget(addBtn, 0, 0);

    QLabel* groupNameTag = new QLabel();
    groupNameTag->setText("Name Of Group");
    groupNameTag->setStyleSheet("QLabel {font-weight: 700; font-size: 16px; }");
    groupNameTag->setFixedHeight(50);
    groupNameTag->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    groupNameTag->setAlignment(Qt::AlignBottom); // Sets the alignment of the text inside QLabel
    vlayout->addWidget(groupNameTag);

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setSpacing(0);
    hlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addLayout(hlayout);

    groupNameLabel = new QLabel();
    groupNameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    groupNameLabel->setFixedHeight(50);
    groupNameLabel->setStyleSheet("QLabel { font-size: 18px; }");
    hlayout->addWidget(groupNameLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);

    QPushButton* modifyBtn = new QPushButton();
    modifyBtn->setFixedSize(30, 30);
    modifyBtn->setIconSize(QSize(20, 20));
    modifyBtn->setIcon(QIcon(":/resource/image/modify.png"));
    modifyBtn->setStyleSheet("QPushButton { border: none; background-color: transparent; } QPushButton:pressed { background-color: rgb(230, 230, 230); }");
    hlayout->addWidget(modifyBtn, 0, Qt::AlignRight | Qt::AlignVCenter);

    QPushButton* exitGroupBtn = new QPushButton();
    exitGroupBtn->setText("Quit Group");
    exitGroupBtn->setFixedHeight(50);
    exitGroupBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QString btnStyle = "QPushButton { border: 1px solid rgb(90, 90, 90); border-radius: 5px; background-color: transparent;}";
    btnStyle += "QPushButton:pressed { background-color: rgb(230, 230, 230); }";
    exitGroupBtn->setStyleSheet(btnStyle);
    vlayout->addWidget(exitGroupBtn);

#if TEST_UI
    groupNameLabel->setText("We are family~");
    QIcon avatar(":/resource/image/defaultAvatar.png");
    for (int i = 0; i < 20; ++i)
    {
        AvatarItem* item = new AvatarItem(avatar, "SnowK" + QString::number(i));
        this->AddMember(item);
    }
#endif

    InitData();
}

void GroupSessionDetailWidget::InitData()
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    connect(dataCenter, &DataCenter::GetMemberListDone, this, &GroupSessionDetailWidget::InitMembers);
    dataCenter->GetMemberListAsync(dataCenter->GetCurrentChatSessionId());
}

void GroupSessionDetailWidget::InitMembers(const QString &chatSessionId)
{
    DataCenter* dataCenter = DataCenter::GetInstance();
    QList<UserInfo>* memberList = dataCenter->GetMemberList(chatSessionId);
    if (memberList == nullptr)
    {
        LOG() << "The obtained member list is empty, chatSessionId=" << chatSessionId;
        return;
    }

    for (const auto& u : *memberList)
    {
        AvatarItem* avatarItem = new AvatarItem(u.avatar, u.nickname);
        this->AddMember(avatarItem);
    }

    groupNameLabel->setText("New group chat");
}

void GroupSessionDetailWidget::AddMember(AvatarItem *avatarItem)
{
    const int MAX_COL = 4;
    if (curCol >= MAX_COL)
    {
        ++curRow;
        curCol = 0;
    }

    glayout->addWidget(avatarItem, curRow, curCol);
    ++curCol;
}
