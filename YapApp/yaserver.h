#ifndef YASERVER_H
#define YASERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QUuid>

class YaServer : public QTcpServer
{
    Q_OBJECT

public:
    YaServer(QObject* parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
    QString generateToken();
};

#endif // YASERVER_H
