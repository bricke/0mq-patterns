#ifndef QUEUE_H
#define QUEUE_H

#include <QObject>
#include <QQueue>
#include <QList>
#include <QByteArray>
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

    struct Worker_t {
        QByteArray identity;     //  Address of worker
        qint64     expiry;       //  Expires at this time

        inline bool operator==(const Worker_t &worker) {
            return (worker.identity == this->identity);
        }
    };

    void *context;
    void *frontend;
    void *backend;
    zmq_pollitem_t items[2];
    QQueue<Worker_t> workerQueue;
    QList<QByteArray> readAll(void *socket);
    void addWorkerToQueue(QByteArray identity);
    void removeWorkerFromQueue(QByteArray identity);
    void purgeExpiredWorkers();
    void workerAlive(QByteArray identity);
};

#endif // QUEUE_H
