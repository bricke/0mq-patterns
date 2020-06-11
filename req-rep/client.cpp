#include "client.h"
#include <zmq.h>
#include <QDebug>

Client::Client(QObject *parent) : QObject(parent)
{
    //Using new context, inproc not available
    context = zmq_ctx_new();
    req = zmq_socket(context, ZMQ_REQ);
}

Client::~Client()
{
    zmq_close (req);
    zmq_ctx_destroy(context);
}

//BLOCKING!!!
QByteArray Client::sendRequest(const QString server, const QByteArray data)
{
    int rc = zmq_connect(req, server.toLatin1().data());

    if (rc != 0){
        qDebug() << "Connect failed";
        return QByteArray();
    }
    zmq_send(req, data.data(), static_cast<size_t>(data.size()), 0);

    QByteArray response;
    response.reserve(1000);
    int bytes = zmq_recv (req, response.data(), 1000, 0);
    response.resize(bytes);
    return response;
}
