#include "mainwidget.h"
#include "loginwidget.h"

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

    return a.exec();
}
