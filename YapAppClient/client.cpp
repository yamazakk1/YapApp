#include "client.h"

Client &Client::getInstance()
{
    static Client instance;
    return instance;
}

void Client::Connect(QString ip, int port)
{
    socket->connectToHost(ip, port);
}

Client::~Client()
{

}

Client::Client()
{
    socket = new QTcpSocket();
    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &Client::onError);
}

void Client::onConnected()
{
    qDebug() << "Connected to server";
    emit OnConnected();
}

void Client::onReadyRead()
{
    QByteArray response = socket->readAll();
    qDebug() << "Response from server:\n" << response;
}

void Client::onDisconnected()
{
    qDebug() << "Disconected";
}

void Client::onError()
{
    emit OnErrorConnect();
}


