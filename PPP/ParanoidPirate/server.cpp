#include "server.h"
#include <QDateTime>
#include <QDebug>
#include <unistd.h>

#define LIVENESS          3
#define HEARTBEAT_INTERVAL   1000
#define INTERVAL_INIT       1000    //  Initial reconnect
#define INTERVAL_MAX       32000    //  After exponential backoff

Server::Server(QObject *parent) : QObject(parent)
{
    //Using new context, inproc not available
    init();
}

void Server::init()
{
    context = zmq_ctx_new();
    dealer = zmq_socket(context, ZMQ_DEALER);
    qsrand((qulonglong)dealer);
    identity = QStringLiteral("Worker-%1").arg(qrand()).toLocal8Bit();
    zmq_setsockopt(dealer, ZMQ_IDENTITY, identity,
                   static_cast<size_t>(identity.size()));
    zmq_setsockopt (dealer, ZMQ_LINGER, 0, 1);
    qDebug() << "server init " << identity;
    items[0] = {dealer, 0, ZMQ_POLLIN, 0};
}

Server::~Server()
{
    zmq_close (dealer);
    zmq_ctx_destroy(context);
}

void Server::listen(const QString address)
{
    //connect to queue
    int rc = zmq_connect (dealer, address.toLatin1().data());

    if (rc != 0) {
        qDebug() << "Connect failed";
        return;
    }

    zmq_send(dealer, "READY", 5, 0);

    qsrand((qulonglong) dealer);
    int sHappens = qrand() % 100;

    int liveness = LIVENESS;
    int reconnectInterval = INTERVAL_INIT;
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    forever{
        //Wait 1 seconds before exiting
        zmq_poll(items, 1, HEARTBEAT_INTERVAL);
        if (items[0].revents & ZMQ_POLLIN) {
            QList<QByteArray> input = readAll(dealer);

            switch (input.size()) {
            case 1:
                //heartbeat received
                if (QString(input.first()).compare("HEARTBEAT") == 0){
                    liveness = LIVENESS;
                }
            break;
            case 3: {
                QByteArray address = input.first();

                sHappens = qrand() % 100;
                liveness = LIVENESS;

                if (sHappens % 10 == 0) {
                    // sleep 1 seconds before sending a response
                    usleep(1 * 1000 * 1000);
                    zmq_send (dealer, address.data(),
                              static_cast<size_t>(address.size()), ZMQ_SNDMORE);
                    zmq_send (dealer, "World...busy!", 13, 0);
                } else {
                    if (sHappens % 19 == 0){
                        qDebug() << identity <<"CRASH";
                        break;
                    } else {
                        zmq_send (dealer, address.data(),
                                  static_cast<size_t>(address.size()), ZMQ_SNDMORE);
                        zmq_send (dealer, "World!", 6, 0);
                    }
                }
            }
            break;
            default:
                qDebug() << "Mummble mummble";
                continue;
                break;
            }
            reconnectInterval = INTERVAL_INIT;

        } else {
            //haven't received the heartbeat
            if (--liveness == 0) {
                qDebug() << identity << " heartbeat failure, can't reach queue";
                qDebug() << identity << " reconnecting in " << reconnectInterval << " msec…";
                usleep(reconnectInterval*1000);

                if (reconnectInterval < INTERVAL_MAX) {
                    reconnectInterval *= 2;
                }
                zmq_close (dealer);
                zmq_ctx_destroy(context);
                init();
                int rc = zmq_connect (dealer, address.toLatin1().data());
                if (rc != 0) {
                    qDebug() << "Connect failed";
                    return;
                }
                zmq_send(dealer, "READY", 5, 0);
                liveness = LIVENESS;
            }
        }

        //  Send heartbeat to queue if it's time
        if (QDateTime::currentMSecsSinceEpoch() > now + HEARTBEAT_INTERVAL) {
            now = QDateTime::currentMSecsSinceEpoch();
            qDebug() << "I: (" << identity << ") worker heartbeat";
            zmq_send(dealer, "HEARTBEAT", 9, 0);
        }
    }
}

QList<QByteArray> Server::readAll(void *socket)
{
    QList<QByteArray> ret;
    forever {
        //  Process all parts of the message
        zmq_msg_t message;
        zmq_msg_init(&message);
        int size = zmq_msg_recv(&message, socket, 0);

        //  Dump the message byte array
        ret.append(QByteArray(static_cast<char*>(zmq_msg_data(&message)), size));
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
