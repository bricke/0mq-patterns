#include "server.h"
#include <zmq.h>
#include <QDebug>

Server::Server(QObject *parent) : QObject(parent)
{
    //Using new context, inproc not available
    context = zmq_ctx_new();
    rep = zmq_socket(context, ZMQ_REP);
}

Server::~Server()
{
    zmq_close (rep);
    zmq_ctx_destroy(context);
}

void Server::listen(const QString address)
{
    int rc = zmq_bind (rep, address.toLatin1().data());

    if (rc != 0) {
        qDebug() << "Bind failed";
        return;
    }

    while(true){
        QByteArray input;
        input.reserve(1000);
        //Blocking
        int bytes = zmq_recv(rep, input.data(), 1000, 0);
        input.resize(bytes);

        if (QString(input).compare("Hello", Qt::CaseInsensitive) == 0)
            zmq_send (rep, "World", 5, 0);
        else
            zmq_send (rep, "not quite sure what you need", 28, 0);
    }
}
