#include "publisher.h"
#include <zmq.h>
#include <QDebug>

Publisher::Publisher(QObject *parent) : QObject(parent)
{
    context = zmq_ctx_new();
    pub = zmq_socket(context, ZMQ_PUB);
}

Publisher::~Publisher()
{
    zmq_close(pub);
    zmq_ctx_destroy(context);
}

bool Publisher::bind(const QString address)
{
    int rc = zmq_bind(pub, address.toLatin1().data());
    if (rc != 0) {
        qDebug() << "Bind failed";
        return false;
    }
    return true;
}

void Publisher::publish(const QString channel, const QByteArray data)
{
    QByteArray message;
    message.append(channel.toLatin1());
    message.append(" ");
    message.append(data);
    zmq_send(pub, message.data(), strlen(message), 0);
}
