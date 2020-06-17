#include "client.h"
#include <zmq.h>
#include <QDebug>

Client::Client(QObject *parent) : QObject(parent)
{
    //Using new context, inproc not available
    context = zmq_ctx_new();
}

Client::~Client()
{
    zmq_close (req);
    zmq_ctx_destroy(context);
}

//Blocking with timeout!
QByteArray Client::sendRequest(const QString server, const QByteArray data, int retries)
{
    req = zmq_socket(context, ZMQ_REQ);

    //  Configure socket to not wait at close time
    int linger = 0;
    zmq_setsockopt(req, ZMQ_LINGER, &linger, sizeof (linger));
    items[0] = {req, 0, ZMQ_POLLIN, 0};

    int rc = zmq_connect(req, server.toLatin1().data());

    if (rc != 0){
        qDebug() << "Connect failed";
        return QByteArray();
    }

    zmq_send(req, data.data(), static_cast<size_t>(data.size()), 0);
    QByteArray ret;

    while (--retries != 0 && ret.isEmpty()) {
        ret = poll(1*1000);
    }
    zmq_disconnect(req, server.toLatin1().data());
    zmq_close (req);
    return ret;
}

//Blocking!
QByteArray Client::poll(int timeout)
{
    zmq_poll(items, sizeof(items)/sizeof(zmq_pollitem_t), timeout);
    if (items[0].revents & ZMQ_POLLIN) {
        QByteArray content = readAll(req);
        return content;
    }
    return QByteArray();
}

QByteArray Client::readAll(void *socket)
{
    QByteArray ret;
    forever {
        //  Process all parts of the message
        zmq_msg_t message;
        zmq_msg_init(&message);
        int size = zmq_msg_recv(&message, socket, 0);

        //  Dump the message byte array
        ret.append(static_cast<char*>(zmq_msg_data(&message)), size);
        zmq_msg_close(&message);

        //  Multipart detection
        int more = 0;
        size_t more_size = sizeof (more);
        zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_size);
        if (!more) {
            break;
        }
    }
    return ret;
}
