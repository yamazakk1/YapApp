#ifndef YAHTTPSERVER_H
#define YAHTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QUuid>
#include <QJsonObject>
#include <QJsonDocument>
#include "httprequest.h"

class YaHttpServer : public QTcpServer
{
    Q_OBJECT

public:
    YaHttpServer(QObject* parent = nullptr);
    static QString generateToken();
protected:
    void incomingConnection(qintptr socketDescriptor) override;
signals:
    void newRequest(QTcpSocket& socket, HttpRequest& request);
    void disconnect(QTcpSocket& socket);
};

#endif // YAHTTPSERVER_H
