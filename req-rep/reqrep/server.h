#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QPointer>
#include <QByteArray>
#include <QString>
#include <zmq.h>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
    void listen(const QString address);

signals:

public slots:

private:
    void *context;
    void *rep;
};

#endif // SERVER_H
