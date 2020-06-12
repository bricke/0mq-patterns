#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <QObject>
#include <QTimer>
#include <zmq.h>

class Subscriber : public QObject
{
    Q_OBJECT
public:
    explicit Subscriber(QObject *parent = nullptr);
    ~Subscriber();

signals:
    void messageReceived(const QString &channel, const QByteArray &msg);

public slots:
    bool connect(const QString address);
    void subscribe(const QString channel);
    void unsubscribe(const QString channel);

private:
    void* context;
    void* sub;
    QTimer pollTimer;
    zmq_pollitem_t items[1];
    void poll();
    QByteArray readAll(void* socket);
};

#endif // SUBSCRIBER_H
