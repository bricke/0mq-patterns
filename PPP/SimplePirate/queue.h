#ifndef QUEUE_H
#define QUEUE_H

#include <QObject>
#include <QQueue>
#include <zmq.h>

class Queue : public QObject
{
    Q_OBJECT
public:
    explicit Queue(QObject *parent = nullptr);
    void bind(const QString frontend, const QString backend);
    void start();
    ~Queue();

signals:

public slots:

private:
    void *context;
    void *frontend;
    void *backend;
    zmq_pollitem_t items[2];
    QQueue<QByteArray> workerQueue;
    QByteArray readAll(void *socket);
    QByteArray readOne(void *socket);
};

#endif // QUEUE_H
