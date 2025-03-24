#ifndef YASERVER_H
#define YASERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>
#include "httprequest.h"
#include "user.h"
#include "yahttpserver.h"

class YaServer : public QObject
{
    Q_OBJECT
public:
    explicit YaServer(int port = -1, QObject *parent = nullptr);
    ~YaServer();
    void startServer(int port);
public slots:
    void newRequest(QTcpSocket& socket, HttpRequest& request);
    void disconnect(QTcpSocket& socket);
private:
    YaHttpServer* httpServer = nullptr;
};
#endif // YASERVER_H
