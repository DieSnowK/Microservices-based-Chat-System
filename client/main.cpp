#include <QApplication>

#include "debug.h"
#include "login_widget.h"

#if DEPLOY == 0
    #include "datacenter.h"
    #include "netclient.h"
    #include "mainwidget.h"
#endif

FILE* output = nullptr;
void MsgHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    (void) type;
    (void) context;

    fprintf(output, "%s\n", msg.toUtf8().constData());
    fflush(output);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if DEPLOY
    output = fopen("./log.txt", "a");
    qInstallMessageHandler(MsgHandler);
#endif

#if TEST_SKIP_LOGIN
    MainWidget* w = MainWidget::GetInstance();
    w->show();
#else
    LoginWidget* loginWidget = new LoginWidget(nullptr);
    loginWidget->show();
#endif

#if TEST_NETWORK
    // network::NetClient netClient(nullptr);
    // netClient.Ping();

    model::DataCenter* dataCenter = model::DataCenter::GetInstance();
    dataCenter->Ping();
#endif

    return a.exec();
}
