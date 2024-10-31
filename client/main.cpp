#include "mainwidget.h"
#include "loginwidget.h"
#include "model/datacenter.h"
#include "network/netclient.h"
#include "debug.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if TEST_SKIP_LOGIN
    MainWidget* w = MainWidget::GetInstance();
    w->show();
#else
    LoginWidget* loginWidget = new LoginWidget(nullptr);
    loginWidget->show();
#endif

#if TEST_NETWORK
    network::NetClient netClient(nullptr);
    netClient.Ping();

    // model::DataCenter* dataCenter = model::DataCenter::getInstance();
    // dataCenter->Ping();
#endif

    return a.exec();
}
