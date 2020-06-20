#ifndef SERVER_H
#define SERVER_H

#include <QObject>
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
    void *req;
    QByteArray identity;
    QByteArray readAll(void *socket);
    zmq_pollitem_t items[1];
};

#endif // SERVER_H
