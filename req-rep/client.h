#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QPointer>
#include <QByteArray>
#include <QString>
#include <zmq.h>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

signals:

public slots:
    QByteArray sendRequest(const QString server, QByteArray data);

private:
    void *context;
    void *req;
};

#endif // CLIENT_H
