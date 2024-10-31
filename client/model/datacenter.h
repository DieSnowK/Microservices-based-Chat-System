#ifndef DATACENTER_H
#define DATACENTER_H

#include <QObject>

namespace model
{
    class DataCenter : public QObject
    {
        Q_OBJECT
    public:
        explicit DataCenter(QObject *parent = nullptr);

    signals:
    };
} // end of namespace model

#endif // DATACENTER_H
