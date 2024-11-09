#include "widget.h"
#include "./ui_widget.h"
#include "server.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    WebsocketServer* websocketServer = WebsocketServer::GetInstance();
    emit websocketServer->SendTextResp();
}


void Widget::on_pushButton_2_clicked()
{
    WebsocketServer* websocketServer = WebsocketServer::GetInstance();
    emit websocketServer->SendFriendRemove();
}


void Widget::on_pushButton_3_clicked()
{
    WebsocketServer* websocketServer = WebsocketServer::GetInstance();
    emit websocketServer->SendAddFriendApply();
}


void Widget::on_pushButton_4_clicked()
{
    WebsocketServer* websocketServer = WebsocketServer::GetInstance();
    emit websocketServer->SendAddFriendProcess(true);
}


void Widget::on_pushButton_5_clicked()
{
    WebsocketServer* websocketServer = WebsocketServer::GetInstance();
    emit websocketServer->SendAddFriendProcess(false);
}


void Widget::on_pushButton_6_clicked()
{
    WebsocketServer* websocketServer = WebsocketServer::GetInstance();
    emit websocketServer->SendCreateChatSession();
}


void Widget::on_pushButton_7_clicked()
{
    WebsocketServer* websocketServer = WebsocketServer::GetInstance();
    emit websocketServer->SendImageResp();
}


void Widget::on_pushButton_8_clicked()
{
    WebsocketServer* websocketServer = WebsocketServer::GetInstance();
    emit websocketServer->SendFileResp();
}

void Widget::on_pushButton_9_clicked()
{
    WebsocketServer* websocketServer = WebsocketServer::GetInstance();
    emit websocketServer->SendSpeechResp();
}

