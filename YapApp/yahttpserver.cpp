#include "yahttpserver.h"
#include "httprequest.h"

YaHttpServer::YaHttpServer(QObject *parent) : QTcpServer(parent)
{
}

void YaHttpServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, [socket, this]()
    {
        QByteArray requestData = socket->readAll();

        QString requestStr(requestData);
        qDebug() << "Socket request on port:\n " << socket->peerPort();

        auto res = HttpRequest::parse(requestData);
        qDebug()
            << "method: " << res.method << "\n"
            << "path: " << res.path << "\n"
            << "quary: " << res.queryParams << "\n"
            << "headers: " << res.headers << "\n"
            << "body: " << res.body << "\n";
        emit newRequest(*socket, res);
    });
    connect(socket, &QTcpSocket::disconnected, [socket, this]()
    {
        emit disconnect(*socket);
    });
}

QString YaHttpServer::generateToken()
{
     return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
