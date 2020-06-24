#include "qzyre.h"
#include <zlist.h>
#include <QDebug>

QZyre::QZyre(QString nodeName, QObject *parent) : QObject(parent)
{
    m_node = zyre_new(nodeName.toLatin1());
    m_listenTimer.setInterval(0);
    m_listenTimer.setSingleShot(false);
    m_listenTimer.start();
#ifdef LOGEVENTS
    printAllEvents();
#endif

    connect(&m_listenTimer, &QTimer::timeout, this, &QZyre::listen);
}

QZyre::~QZyre()
{
    zyre_destroy(&m_node);
}

bool QZyre::start()
{
    if (!zyre_start(m_node)) {
        m_listenTimer.start();
        return true;
    }
    return false;
}

void QZyre::stop()
{
    zyre_stop(m_node);
    m_listenTimer.stop();
}

QString QZyre::getUUID() const
{
    return QString(zyre_uuid(m_node));
}

void QZyre::setUDPBeaconingPort(const int port) const
{
    zyre_set_port(m_node, port);
}

void QZyre::setUDPInterface(const QString &interface) const
{
    zyre_set_interface(m_node, interface.toLatin1().constData());

}

bool QZyre::join(const QString group)
{
    zyre_join(m_node, group.toLatin1().constData());
}

bool QZyre::leave(const QString group)
{
    zyre_leave(m_node, group.toLatin1().constData());
}

bool QZyre::shout(const QString group, const QByteArray message)
{
    zyre_shouts(m_node, group.toLatin1().constData(), message.constData());
}

bool QZyre::whisper(const QString uuid, const QByteArray message)
{
    zyre_whispers(m_node, uuid.toLatin1().constData(), message.constData());
}

void QZyre::listen()
{
    zpoller_t *poller = zpoller_new (zyre_socket(m_node), NULL);
    zsock_t *which = (zsock_t *) zpoller_wait (poller, 1);
    if (zpoller_expired(poller) || zpoller_terminated(poller)) {
        zpoller_destroy(&poller);
        return;
    }

    if (which == zyre_socket(m_node)){
        zmsg_t *message = zyre_recv (m_node);
        char *command = zmsg_popstr(message);
        char *peerid = zmsg_popstr (message);
        char *name = zmsg_popstr (message);

        switch (decodeEvent(QString(command))){
            case ENTER: {
                zframe_t *headers_packed = zmsg_pop (message);
                zhash_t *headers = zhash_unpack (headers_packed);
                char *address = zmsg_popstr (message);
                emit event(convertUuid(QString(peerid)), ENTER, QString(name), QString(address));
                zstr_free (&address);
                zframe_destroy (&headers_packed);
                zhash_destroy (&headers);
                }
                break;
            case EVASIVE:
                emit event(convertUuid(QString(peerid)), EVASIVE, QString(name));
                break;
            case SILENT:
                emit event(convertUuid(QString(peerid)), SILENT, QString(name));
                break;
            case EXIT:
                emit event(convertUuid(QString(peerid)), EXIT, QString(name));
                break;
            case JOIN:{
                    char *groupName = zmsg_popstr (message);
                    emit event(convertUuid(QString(peerid)), JOIN, QString(name), QString(groupName));
                    zstr_free (&groupName);
                }
                break;
            case LEAVE:{
                    char *groupName = zmsg_popstr (message);
                    emit event(convertUuid(QString(peerid)), LEAVE, QString(name), QString(groupName));
                    zstr_free (&groupName);
                }
                break;
            case WHISPER: {
                    char *msg = zmsg_popstr(message);
                    emit whisperReceived(convertUuid(QString(peerid)), QByteArray(msg));
                    zstr_free(&msg);
                }
                break;
            case SHOUT: {
                    char *channel = zmsg_popstr(message);
                    char *msg = zmsg_popstr(message);
                    emit shoutReceived(convertUuid(QString(peerid)), QString(channel), QByteArray(msg));
                    zstr_free(&msg);
                    zstr_free(&channel);
                }
                break;
            default:
                break;
        }

        zstr_free (&command);
        zstr_free (&peerid);
        zstr_free (&name);
        zmsg_destroy(&message);
    }
    else {
        qDebug() << "Weird...";
    }
    zpoller_destroy(&poller);
}

QZyre::Event QZyre::decodeEvent(QString eventName)
{
    if (eventName.compare("ENTER", Qt::CaseInsensitive) == 0) return ENTER;
    if (eventName.compare("EVASIVE", Qt::CaseInsensitive) == 0) return EVASIVE;
    if (eventName.compare("SILENT", Qt::CaseInsensitive) == 0) return SILENT;
    if (eventName.compare("EXIT", Qt::CaseInsensitive) == 0) return EXIT;
    if (eventName.compare("JOIN", Qt::CaseInsensitive) == 0) return JOIN;
    if (eventName.compare("LEAVE", Qt::CaseInsensitive) == 0) return LEAVE;
    if (eventName.compare("WHISPER", Qt::CaseInsensitive) == 0) return WHISPER;
    if (eventName.compare("SHOUT", Qt::CaseInsensitive) == 0) return SHOUT;
    return UNKNOWN;
}

QUuid QZyre::convertUuid(QString uuid)
{
    //00000000-0000-0000-0000-000000000000
    return QUuid(uuid.insert(8, '-').insert(13, '-')
                 .insert(18, '-').insert(23, '-'));
}

void QZyre::printAllEvents()
{
    QObject::connect(this, &QZyre::shoutReceived, [=](QUuid origin, QString channel, QByteArray data){
            qDebug() << origin << "SHOUTED" <<  QString(data) << "in" << channel;
    });

    QObject::connect(this, &QZyre::whisperReceived, [=](QUuid origin, QByteArray data){
            qDebug() << origin << "WHISPERED" << QString(data) << "to" << getUUID();
    });

    QObject::connect(this, &QZyre::event, [=](QUuid origin, QZyre::Event event, QString name, QString data){
        switch(event){
        case QZyre::ENTER: qDebug() << origin << "ENTER" << name << data;
            break;
        case QZyre::EVASIVE: qDebug() << origin << "EVASIVE" << name << data;
            break;
        case QZyre::SILENT: qDebug() << origin << "SILENT" << name << data;
            break;
        case QZyre::EXIT: qDebug() << origin << "EXIT" << name << data;
            break;
        case QZyre::JOIN: qDebug() << origin << "JOIN" << name << data;
            break;
        case QZyre::LEAVE: qDebug() << origin << "LEAVE" << name << data;
            break;
        case QZyre::WHISPER: qDebug() << origin << "WHISPER?" << name << data;
            break;
        case QZyre::SHOUT: qDebug() << origin << "SHOUT?" << name << data;
            break;
            default: qDebug() << origin << event << name << data;
            break;
        }
    });
}
