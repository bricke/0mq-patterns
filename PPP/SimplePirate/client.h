#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QTimer>
#include <zmq.h>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

signals:

public slots:
    QByteArray sendRequest(const QString server, QByteArray data, int retries = 5);

private:
    void *context;
    void *req;
    QTimer pollTimer;
    zmq_pollitem_t items[1];

    QByteArray readAll(void* socket);
    QByteArray poll(int timeout);
};

#endif // CLIENT_H
