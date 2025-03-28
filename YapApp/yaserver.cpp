#include "yaserver.h"

void YaServer::newRequest(QTcpSocket &socket, HttpRequest &request)
{
    QJsonDocument json;
    if(request.header("Content-Type" ) == "application/json")
        json = QJsonDocument::fromJson(request.body);


    QByteArray response;
    QByteArray body = "{}";
    response.append("HTTP/1.1 200 OK\r\n");
    response.append("Content-Type: application/json\r\n");
    response.append("Connection: keep-alive\r\n");
    if(request.path == "/login")
    {
        if (json.isObject()) {
            QJsonObject obj = json.object();
            QString name = obj["name"].toString();
            qDebug() << "Name:" << name;
        }
        if(json.object()["name"].toString() == "123")
            body = "{\"token\":\"" + YaHttpServer::generateToken().toUtf8() + "\"}";
    }
    response.append("Content-Length: " + QByteArray::number(body.size()) + "\r\n");
    response.append("\r\n");
    response.append(body);
    socket.write(response);
}

void YaServer::disconnect(QTcpSocket &socket)
{
    socket.deleteLater();
}

YaServer::YaServer(QObject *parent)
{
}

YaServer::~YaServer()
{
    if (httpServer != nullptr)
        delete httpServer;
}

YaServer &YaServer::getInstance()
{
    static YaServer instance;
    return instance;
}

void YaServer::startServer(int port)
{
    if (httpServer != nullptr)
        delete httpServer;
    httpServer = new YaHttpServer();

    if(!httpServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "server is not started";
        qDebug()<<httpServer->errorString();
        return;
    }
    qDebug() << "server is started at port:" << port;
    connect(httpServer, &YaHttpServer::newRequest, this, &YaServer::newRequest);
    connect(httpServer, &YaHttpServer::disconnect, this, &YaServer::disconnect);
}
