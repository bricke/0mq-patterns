#include <QCoreApplication>
#include <QThread>
#include <QDebug>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <client.h>
#include <server.h>
#include <queue.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("SimplePirate");
    parser.addHelpOption();
    parser.addPositionalArgument("objectType",
                                 QCoreApplication::translate("main", "server, client or queue"));

    parser.process(a);

    const QStringList args = parser.positionalArguments();
    // objectType is args.at(0)

    Client *client;
    Server *server;
    Queue *queue;

    if (args.at(0).compare("server") == 0){
        server = new Server();
        server->listen("tcp://localhost:5556");
    }

    if (args.at(0).compare("client") == 0){
        client = new Client();
        int max_msg = 10;
        while (max_msg > 0) {
            qDebug() << client->sendRequest("tcp://localhost:5555", "Hello", 5);
            --max_msg;
        }
    }

    if (args.at(0).compare("queue") == 0) {
        queue = new Queue();
        queue->bind("tcp://*:5555", "tcp://*:5556");
        queue->start();
    }

    return a.exec();
}
