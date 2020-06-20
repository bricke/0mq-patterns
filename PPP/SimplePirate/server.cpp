#include "server.h"
#include <QTime>
#include <QDebug>
#include <unistd.h>

Server::Server(QObject *parent) : QObject(parent)
{
    //Using new context, inproc not available
    context = zmq_ctx_new();
    req = zmq_socket(context, ZMQ_REQ);
    qsrand((qulonglong)req);
    identity = QStringLiteral("Worker-%1").arg(qrand()).toLocal8Bit();
    zmq_setsockopt(req, ZMQ_IDENTITY, identity,
                   static_cast<size_t>(identity.size()));
    qDebug() << "server init " << identity;
    items[0] = {req, 0, ZMQ_POLLIN, 0};
}

Server::~Server()
{
    zmq_close (req);
    zmq_ctx_destroy(context);
}

void Server::listen(const QString address)
{
    //connect to queue
    int rc = zmq_connect (req, address.toLatin1().data());

    if (rc != 0) {
        qDebug() << "Connect failed";
        return;
    }

    zmq_send (req, "noAddr", 6, ZMQ_SNDMORE);
    zmq_send(req, "READY", 5, 0);

    qsrand((qulonglong) req);
    int sHappens = qrand() % 100;

    forever{
        //Blocking
        zmq_poll(items, 1, -1);
        if (items[0].revents & ZMQ_POLLIN) {
            QByteArray input = readAll(req);

            QString tmp = QString(input).split("-Client").first();
            tmp.append("-Client");
            QByteArray address(tmp.toLatin1());

            sHappens = qrand() % 100;

            if (sHappens % 10 == 0) {
                // sleep 2 seconds before sending a response
                usleep(2 * 1000 * 1000);
                zmq_send (req, address.data(),
                          static_cast<size_t>(address.size()), ZMQ_SNDMORE);
                zmq_send (req, "World...busy!", 13, 0);
            } else {
                if (sHappens % 13 == 0){
                    qDebug() << identity <<"CRASH";
                    break;
                } else {
                    zmq_send (req, address.data(),
                              static_cast<size_t>(address.size()), ZMQ_SNDMORE);
                    zmq_send (req, "World!", 6, 0);
                }
            }
        }
    }
}

QByteArray Server::readAll(void *socket)
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
