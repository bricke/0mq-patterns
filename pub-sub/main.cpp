#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include <QDebug>

#include <publisher.h>
#include <subscriber.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Publisher *pub = new Publisher();
    pub->bind("tcp://*:5555");
    QTimer pubTimer;
    pubTimer.setInterval(1*1000);
    pubTimer.setSingleShot(false);
    QObject::connect(&pubTimer, &QTimer::timeout, [=](){

        if (qrand()%3)
            pub->publish("CHANNEL2", "Content for channel 2");

        if (qrand()%5)
            pub->publish("CHANNEL1", "Content for channel 1");

        if (qrand()%7)
            pub->publish("CHANNEL3", "Content for channel 3");
    });

    //First Subscriber
    Subscriber *sub1 = new Subscriber();
    sub1->connect("tcp://localhost:5555");
    sub1->subscribe("CHANNEL1");
    sub1->subscribe("CHANNEL3");

    QObject::connect(sub1, &Subscriber::messageReceived, [=](QString channel, QByteArray data){
        qDebug() << "SUB1 = " << channel << data;
    });

    //Second Subscriber
    Subscriber *sub2 = new Subscriber();
    sub2->connect("tcp://localhost:5555");
    sub2->subscribe("CHANNEL2");

    QObject::connect(sub2, &Subscriber::messageReceived, [=](QString channel, QByteArray data){
        qDebug() << "SUB2 = " << channel << data;
    });

    pubTimer.start();

    return a.exec();
}
