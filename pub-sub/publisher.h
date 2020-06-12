#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <QObject>

class Publisher : public QObject
{
    Q_OBJECT
public:
    explicit Publisher(QObject *parent = nullptr);
    ~Publisher();

signals:

public slots:
    bool bind(const QString address);
    void publish(const QString channel, const QByteArray data);

private:
    void *context;
    void *pub;
};

#endif // PUBLISHER_H
