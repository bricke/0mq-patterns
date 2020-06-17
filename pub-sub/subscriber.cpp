#include "subscriber.h"

Subscriber::Subscriber(QObject *parent) : QObject(parent)
{
    pollTimer.setInterval(100);
    pollTimer.setSingleShot(false);
    QObject::connect(&pollTimer, &QTimer::timeout, this, &Subscriber::poll);

    context = zmq_ctx_new();
    sub = zmq_socket(context, ZMQ_SUB);
    items[0] = {sub, 0, ZMQ_POLLIN, 0};
}

Subscriber::~Subscriber()
{
    pollTimer.stop();
    zmq_close(sub);
    zmq_ctx_destroy(context);
}

bool Subscriber::connect(const QString address)
{
    bool ret = !zmq_connect(sub, address.toLatin1().data());
    if (ret) pollTimer.start();
    return ret;
}

void Subscriber::subscribe(const QString channel)
{
    zmq_setsockopt(sub, ZMQ_SUBSCRIBE, channel.toLatin1().data(),
                   static_cast<size_t>(channel.size()));
}

void Subscriber::unsubscribe(const QString channel)
{
    zmq_setsockopt(sub, ZMQ_UNSUBSCRIBE, channel.toLatin1().data(),
                   static_cast<size_t>(channel.size()));
}

void Subscriber::poll()
{
    zmq_poll(items, sizeof(items)/sizeof(zmq_pollitem_t), 5);
    if (items[0].revents & ZMQ_POLLIN) {
        QByteArray content = readAll(sub);
        const QString channel = QLatin1String(content.left(content.indexOf(' ')));
        const QByteArray message = content.remove(0, channel.size()+1);
        emit messageReceived(channel, message);
    }
}

QByteArray Subscriber::readAll(void *socket)
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
