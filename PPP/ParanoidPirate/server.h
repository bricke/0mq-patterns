#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QByteArray>
#include <zmq.h>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void init();
    ~Server();
    void listen(const QString address);

signals:

public slots:

private:
    void *context;
    void *dealer;
    QByteArray identity;
    zmq_pollitem_t items[1];
    QList<QByteArray> readAll(void *socket);
};

#endif // SERVER_H
