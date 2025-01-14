#include <QPainter>

#include "verifycode_widget.h"
#include "data.hpp"

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
    verifyCode = GenerateVerifyCode();

    // Refresh the interface and trigger paintEvent
    this->update();
}

bool VerifyCodeWidget::CheckVerifyCode(const QString &verifyCode)
{
    // When comparing captchas here, need to ignore case
    return this->verifyCode.compare(verifyCode, Qt::CaseInsensitive) == 0;
}

void VerifyCodeWidget::paintEvent(QPaintEvent *event)
{
    (void) event;
    const int width = 180;
    const int height = 80;

    QPainter painter(this);
    QPen pen;
    QFont font("楷体", 25,QFont::Bold, true);
    painter.setFont(font);

    // Point: Add random noise
    for(int i = 0; i < 100; i++)
    {
        pen = QPen(QColor(randomGenerator.generate() % 256,
                          randomGenerator.generate() % 256, randomGenerator.generate() % 256));
        painter.setPen(pen);
        painter.drawPoint(randomGenerator.generate() % width, randomGenerator.generate() % height);
    }

    // Line: Add random interference lines
    for(int i = 0; i < 5; i++)
    {
        pen = QPen(QColor(randomGenerator.generate() % 256,
                          randomGenerator.generate() % 256, randomGenerator.generate() % 256));
        painter.setPen(pen);
        painter.drawLine(randomGenerator.generate() % width, randomGenerator.generate() % height,
                         randomGenerator.generate() % width, randomGenerator.generate() % height);
    }

    // Draw verification code
    for(int i = 0; i < verifyCode.size(); i++)
    {
        pen = QPen(QColor(randomGenerator.generate() % 255,
                          randomGenerator.generate() % 255, randomGenerator.generate() % 255));
        painter.setPen(pen);
        painter.drawText(5 + 20 * i, randomGenerator.generate() % 10,
                         30, 30, Qt::AlignCenter, QString(verifyCode[i]));
    }
}

void VerifyCodeWidget::mousePressEvent(QMouseEvent *event)
{
    (void) event;
    this->RefreshVerifyCode();
}
