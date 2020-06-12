#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <QObject>

class Subscriber : public QObject
{
    Q_OBJECT
public:
    explicit Subscriber(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SUBSCRIBER_H