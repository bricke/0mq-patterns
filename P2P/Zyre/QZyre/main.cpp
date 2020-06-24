#include <QCoreApplication>
#include <qzyre.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QZyre nodeOne("Node1");
    nodeOne.setUDPBeaconingPort(123456);
    nodeOne.setUDPInterface("*");
    nodeOne.start();

    QZyre nodeTwo("Node2");
    nodeTwo.setUDPBeaconingPort(123456);
    nodeTwo.setUDPInterface("*");
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
