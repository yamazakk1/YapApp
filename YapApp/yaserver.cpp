#include "yaserver.h"
#include "httprequest.h"

YaServer::YaServer(QObject *parent) : QTcpServer(parent)
{
}

void YaServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, [socket, this]() {
        QByteArray requestData = socket->readAll();

        QString requestStr(requestData);
        qDebug() << "Получен запрос нв:\n " << socket->peerPort();

        auto res = HttpRequest::parse(requestData);
        qDebug()
                << "method: " << res.method << "\n"
            << "path: " << res.path << "\n"
            << "quary: " << res.queryParams << "\n"
            << "headers: " << res.headers << "\n"
            << "body: " << res.body << "\n";

        QByteArray body = "{\"token\":\"" + generateToken().toUtf8() + "\"}";
        QByteArray response;
        response.append("HTTP/1.1 200 OK\r\n");
        response.append("Content-Type: application/json\r\n");
        response.append("Connection: keep-alive\r\n");
        response.append("Content-Length: " + QByteArray::number(body.size()) + "\r\n");
        response.append("\r\n");
        response.append(body);

        socket->write(response);

    });

    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

}

QString YaServer::generateToken()
{
     return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
