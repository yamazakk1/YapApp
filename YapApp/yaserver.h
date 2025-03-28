#ifndef YASERVER_H
#define YASERVER_H

#include <QObject>
#include <QTcpSocket>
#include "yahttpserver.h"
#include "database.h"

class YaServer : public QObject
{
    Q_OBJECT
public:
    static YaServer& getInstance();

    void startServer(int port);
    YaServer(const YaServer&) = delete;
    YaServer& operator=(const YaServer&) = delete;
private slots:
    void newRequest(QTcpSocket &socket, HttpRequest &request);
    void disconnect(QTcpSocket &socket);

private:
    explicit YaServer(QObject *parent = nullptr);
    ~YaServer();

    YaHttpServer *httpServer = nullptr;
};

#endif // YASERVER_H
