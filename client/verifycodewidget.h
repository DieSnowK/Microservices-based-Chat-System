#ifndef VERIFYCODEWIDGET_H
#define VERIFYCODEWIDGET_H

#include <QWidget>
#include <QRandomGenerator>

class VerifyCodeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VerifyCodeWidget(QWidget *parent = nullptr);

    QString GenerateVerifyCode();
    void RefreshVerifyCode();
    bool CheckVerifyCode(const QString& verifyCode);

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QRandomGenerator randomGenerator;
    QString verifyCode = "";
};

#endif // VERIFYCODEWIDGET_H
