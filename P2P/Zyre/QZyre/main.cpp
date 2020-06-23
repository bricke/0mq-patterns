#include <QCoreApplication>
#include <qzyre.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QZyre nodeOne("Node1");
    nodeOne.setGossipEndpoint("tcp://127.0.0.1:123456");
    nodeOne.gossipBindTo("tcp://127.0.0.1:55555");
    nodeOne.start();

    QZyre nodeTwo("Node2");
    nodeTwo.setGossipEndpoint("tcp://127.0.0.1:456798");
    nodeTwo.gossipConnect("tcp://127.0.0.1:55555");
    nodeTwo.start();

    qDebug() << "Node1 =" << nodeOne.getUUID();
    qDebug() << "Node2 =" << nodeTwo.getUUID();

    nodeOne.join("GLOBAL");
    nodeTwo.join("GLOBAL");


    qDebug() << "Waiting 1.5s for discovery";
    zclock_sleep (1500);

    nodeTwo.shout("GLOBAL", "Lorem ipsum dolor sit amet");
    nodeOne.shout("GLOBAL", "Latin?!?!");
    nodeTwo.whisper(nodeOne.getUUID(), "Hey You!");
    nodeOne.whisper(nodeTwo.getUUID(), "What's up?");

    return a.exec();
}
