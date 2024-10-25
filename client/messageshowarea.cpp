#include "messageshowarea.h"

MessageShowArea::MessageShowArea() 
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setWidgetResizable(true);
    this->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 2px; \
                                              background-color: rgb(240, 240, 240); }");
    this->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal { height: 0; }");
    this->setStyleSheet("QScrollArea { border: none; }");

    container = new QWidget();
    this->setWidget(container);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    container->setLayout(layout);

#if TEST_UI
    UserInfo userInfo;
    userInfo.userId = QString::number(1000);
    userInfo.nickname = "SnowK";
    userInfo.description = "Cool Boy~";
    userInfo.avatar = QIcon(":/resource/image/defaultAvatar.png");
    userInfo.phone = "18351958129";
    Message message = Message::MakeMessage(TEXT_TYPE, "", userInfo, QString("This is a test message").toUtf8(), "");
    this->addMessage(false, message);

    for (int i = 1; i <= 30; ++i)
    {
        UserInfo userInfo;
        userInfo.userId = QString::number(1000 + i);
        userInfo.nickname = "SnowK" + QString::number(i);
        userInfo.description = "Cool Boy~";
        userInfo.avatar = QIcon(":/resource/image/defaultAvatar.png");
        userInfo.phone = "18351958129";
        Message message = Message::MakeMessage(TEXT_TYPE, "", userInfo, (QString("This is a test message") + QString::number(i)).toUtf8(), "");
        this->addMessage(true, message);
    }
#endif
}
