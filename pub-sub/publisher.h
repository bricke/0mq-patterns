#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <QObject>

class Publisher : public QObject
{
    Q_OBJECT
public:
    explicit Publisher(QObject *parent = nullptr);

signals:

public slots:
};

#endif // PUBLISHER_H