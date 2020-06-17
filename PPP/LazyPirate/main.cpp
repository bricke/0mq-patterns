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
        qDebug() << "Client:" << "HELLO";
        qDebug() << "Server:" << clt->sendRequest("tcp://localhost:5555", "HELLO");

        qDebug() << "Client:" << "What's up?";
        qDebug() << "Server:" << clt->sendRequest("tcp://localhost:5555", "What's up?");

        qDebug() << "";
        bool success = false;
        for (int i=0; i<3; i++){
            QByteArray response = clt->sendRequest("tcp://localhost:5555", "sim crash");
            if (!response.isEmpty()) {
                qDebug() << "Client:" << "sim crash"
                         << "Server:" << response;
                success = true;
            } else {
                qDebug() << "Client: sim crash - - Server not responding";
            }
        }
        if (success)
            qDebug() << "Server didn't crash! mummble mummble";
        else
            qDebug() << "Server not responding - abort";

    });
    workerClt->start();

    return a.exec();
}
