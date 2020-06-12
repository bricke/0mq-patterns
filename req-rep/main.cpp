#include <QCoreApplication>
#include <QThread>
#include <QDebug>

#include <client.h>
#include <server.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //Start Server
    Server *srv = new Server();
    QThread *workerSrv = new QThread();
    srv->moveToThread(workerSrv);

    QObject::connect(workerSrv, &QThread::started, [=](){
        srv->listen("tcp://*:5555");
    });
    workerSrv->start();

    //Start Client
    Client *clt = new Client();
    QThread *workerClt = new QThread();
    clt->moveToThread(workerClt);

    QObject::connect(workerClt, &QThread::started, [=](){
        qDebug() << "Client:" << "HELLO"
                 << "Server:" << clt->sendRequest("tcp://localhost:5555", "HELLO");
        qDebug() << "Client:" << "What's up?"
                 << "Server:" << clt->sendRequest("tcp://localhost:5555", "What's up?");
    });
    workerClt->start();

    return a.exec();
}
