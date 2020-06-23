#ifndef QZYRE_H
#define QZYRE_H

#include <QObject>
#include <QStringList>
#include <QTimer>
#include <QUuid>
#include <zyre/zyre.h>

//ENTER fromnode name headers ipaddress:port
//    a new peer has entered the network
//EVASIVE fromnode name
//    a peer is being evasive (i.e. quiet) and will be pinged manually
//SILENT fromnode name
//    a peer has been quiet and has not answered ping after 1 second
//EXIT fromnode name
//    a peer has left the network
//JOIN fromnode name groupname
//    a peer has joined a specific group
//LEAVE fromnode name groupname
//    a peer has joined a specific group
//WHISPER fromnode name message
//    a peer has sent this node a message
//SHOUT fromnode name groupname message
//    a peer has sent one of our groups a message


class QZyre : public QObject
{
    Q_OBJECT
public:

    typedef enum {
        ENTER,
        EVASIVE,
        SILENT,
        EXIT,
        JOIN,
        LEAVE,
        WHISPER,
        SHOUT,
        UNKNOWN
    } Event;

    explicit QZyre(QString nodeName, QObject *parent = nullptr);
    ~QZyre();

    bool start();
    void stop();

    QString getUUID() const;

    void setGossipEndpoint(const QString endpoint) const;
    void gossipBindTo(const QString endpoint) const;
    void gossipConnect(const QString endpoint) const;

    bool join(const QString group);
    bool leave(const QString group);

    bool shout(const QString group, const QByteArray message);
    bool whisper(const QString uuid, const QByteArray message);

signals:
    void shoutReceived(QUuid origin, QString channel, QByteArray data);
    void whisperReceived(QUuid origin, QByteArray data);
    void event(QUuid origin, Event event, QString name, QString data = "");

private:
    zyre_t *m_node;
    void listen();
    QTimer m_listenTimer;
    Event decodeEvent(QString eventName);
    QUuid convertUuid(QString uuid);
    void printAllEvents();


public slots:
};

#endif // QZYRE_H
