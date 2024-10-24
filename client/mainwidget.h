#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui 
{
    class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

private:
    static MainWidget* instance;
    MainWidget(QWidget *parent = nullptr);

public:
    ~MainWidget();
    static MainWidget* GetInstance();

private:
    Ui::MainWidget *ui;
};
#endif // MAINWIDGET_H
