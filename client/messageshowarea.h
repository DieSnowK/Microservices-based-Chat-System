#ifndef MESSAGESHOWAREA_H
#define MESSAGESHOWAREA_H

#include <QScrollArea>
#include <QWidget>
#include <QScrollBar>
#include <QVBoxLayout>

class MessageShowArea : public QScrollArea
{
    Q_OBJECT
public:
    MessageShowArea();

private:
    QWidget *container;
};

#endif // MESSAGESHOWAREA_H
