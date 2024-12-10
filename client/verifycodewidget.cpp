#include "verifycodewidget.h"
#include "model/data.hpp"

VerifyCodeWidget::VerifyCodeWidget(QWidget *parent)
    : QWidget{parent}
    , randomGenerator(model::Util::GetTime())
{
    verifyCode = GenerateVerifyCode();
}

QString VerifyCodeWidget::GenerateVerifyCode()
{
    QString code;
    for (int i = 0; i < 4; ++i)
    {
        char init = 'A';
        init += randomGenerator.generate() % 26;
        code += static_cast<QChar>(init);
    }

    return code;
}

void VerifyCodeWidget::RefreshVerifyCode()
{

}

bool VerifyCodeWidget::CheckVerifyCode(const QString &verifyCode)
{

}

void VerifyCodeWidget::paintEvent(QPaintEvent *event)
{

}

void VerifyCodeWidget::mousePressEvent(QMouseEvent *event)
{

}
