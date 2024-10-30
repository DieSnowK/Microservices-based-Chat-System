#include "toast.h"

Toast::Toast(const QString &text)
{
    this->setFixedSize(800, 150);
    this->setWindowTitle("Message Notifications");
    this->setWindowIcon(QIcon(":/resource/image/logo.png"));
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowOpacity(0.7);
    this->setWindowFlags(Qt::FramelessWindowHint);

    // The location of the window
    QScreen* screen = QApplication::primaryScreen();
    int width = screen->size().width();
    int height = screen->size().height();
    int x = (width - this->width()) / 2;
    int y = height - this->height() - 100;	// 100 ->  bottom edge of the window
    this->move(x, y);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    QLabel* label = new QLabel();
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("QLabel { font-size: 32px; }");
    label->setText(text);
    layout->addWidget(label);

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]()
    {
        timer->stop();
        this->close();
    });
    timer->start(2000);
}

void Toast::ShowMessage(const QString &text)
{
    Toast* toast = new Toast(text);
    toast->show();
}
