#include "queue.h"
#include <QDebug>
#include <QList>
#include <czmq.h>


Queue::Queue(QObject *parent) : QObject(parent)
{
    context = zmq_ctx_new();
    frontend = zmq_socket(context, ZMQ_ROUTER);
    backend = zmq_socket(context, ZMQ_ROUTER);
    items[0] = {backend, 0, ZMQ_POLLIN, 0};
    items[1] = {frontend, 0, ZMQ_POLLIN, 0};
    zmq_setsockopt(frontend, ZMQ_ROUTER_MANDATORY, "1", 1);
    zmq_setsockopt(backend, ZMQ_ROUTER_MANDATORY, "1", 1);

}

void Queue::bind(const QString frontend, const QString backend)
{
    int fe = zmq_bind (this->frontend, frontend.toLatin1().data());
    int be = zmq_bind (this->backend, backend.toLatin1().data());

    if (fe != 0 && be != 0) {
        qDebug() << "Bind failed";
        return;
    }
}

//Blocking
void Queue::start()
{
    forever {
        //If I have no available workers
        //poll backend only
        if (workerQueue.size() == 0) {
            zmq_poll(items, 1, -1);
        }
        else {
            zmq_poll(items, 2, -1);
        }

        // Message returning from a worker
        if (items [0].revents & ZMQ_POLLIN) {
            //Reading the worker response

            QByteArray workerAddress = readOne(backend);
            QByteArray empty = readOne(backend);
            Q_UNUSED(empty);
            QByteArray clientAddress = readOne(backend);
            QByteArray message = readOne(backend);

            workerQueue.enqueue(workerAddress);
            qDebug() << "Worker" << workerAddress << "back in queue, queue size" << workerQueue.size();

            //  Return reply to client if it's not a READY
            if (QString(message).compare("READY", Qt::CaseInsensitive) != 0) {
                zmq_send(frontend, clientAddress.data(), static_cast<size_t>(clientAddress.size()), ZMQ_SNDMORE);
                zmq_send(frontend, NULL, 0, ZMQ_SNDMORE);
                zmq_send(frontend, message.data(), static_cast<size_t>(message.size()), 0);
                qDebug() << "\tResponse for" << clientAddress<< "from Worker" << workerAddress;
            }
         }

        //Message coming from a client
        if (items [1].revents & ZMQ_POLLIN) {
            QByteArray address = readOne(frontend);
            QByteArray empty = readOne(frontend);
            Q_UNUSED(empty);
            QByteArray message = readOne(frontend);

            //Forwarding message to backend
            QByteArray to = workerQueue.dequeue();
            zmq_send(backend, to.data(), static_cast<size_t>(to.size()), ZMQ_SNDMORE);
            zmq_send(backend, NULL, 0, ZMQ_SNDMORE);
            zmq_send(backend, address.data(), static_cast<size_t>(to.size()), ZMQ_SNDMORE);
            zmq_send(backend, message.data(), static_cast<size_t>(message.size()), 0);
            qDebug() << "\tForwarding"<< address << "to" << to;
        }
    }
}

Queue::~Queue()
{
    zmq_close (frontend);
    zmq_close (backend);
    zmq_ctx_destroy(context);
}

QByteArray Queue::readAll(void *socket)
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

QByteArray Queue::readOne(void *socket)
{
    zmq_msg_t message;
    zmq_msg_init(&message);
    int size = zmq_msg_recv(&message, socket, 0);
    QByteArray ret(static_cast<char*>(zmq_msg_data(&message)), size);
    zmq_msg_close(&message);
    return ret;
}


