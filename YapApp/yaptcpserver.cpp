#include "yaptcpserver.h"
#include <QDebug>
#include <QCoreApplication>
#include<QString>

YapTcpServer::YapTcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &YapTcpServer::connectSocket);

    if(!mTcpServer->listen(QHostAddress::Any, 33333))
    {
        qDebug() << "server is not started";
    }
    else
    {
        qDebug() << "server is started";
    }
}

YapTcpServer::YapConnection& YapTcpServer::getConnection(QTcpSocket *socket)
{
    return mSockets[socket->socketDescriptor()];
}

QString YapTcpServer::getSocketName(QTcpSocket *socket)
{

    auto res = socket->peerAddress().toString() + ":" + QString::number(socket->peerPort());
    auto u = getConnection(socket).user;
    if(u != nullptr)
        res += "(" + u->getFirstName() + " " + u->getLastName() + ")";
    return res;
}

QString YapTcpServer::ReadQString(QTcpSocket *socket)
{
    QString res = "";
    while(socket->bytesAvailable()>0)
    {
        QByteArray array = socket->readAll();
        res.append(array);
    }
    qDebug() << getSocketName(socket) << " send: " << res;
    return res;
}

void YapTcpServer::readSocket()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    QString message = ReadQString(socket);
    auto user = getConnection(socket).user;
    if(user == nullptr){
        registUser(socket, message);
        return;
    }
    socket->write("Send!\r\n");
    for (auto i : mSockets) {
        if(i.user == nullptr || i.user == user) continue;
        i.socket->write((user->getFirstName() + " " + user->getLastName() +
                       " send: " + message).toLocal8Bit()+"\r");
    }
}

void YapTcpServer::connectSocket(){
    QTcpSocket* socket = mTcpServer->nextPendingConnection();
    registerSoket(socket);
    qDebug() << "NEW CONECTION";
    qDebug() << getSocketName(socket);
    registUser(socket);
}

void YapTcpServer::registUser(QTcpSocket *socket, QString message)
{
    auto parts = message.split('\\');
    User* user;
    if(parts.length() == 4 && parts.first().trimmed() == "reg")
    {
        user = new User(parts[1].trimmed(), parts[2].trimmed(), parts[3].trimmed());
        getConnection(socket).user = user;
        socket->write("Register successful!\r\n");
        return;
    }
    socket->write("PLS reg\\name\\last name\\email\r\n");
}

void YapTcpServer::registerSoket(QTcpSocket* socket)
{
    mSockets[socket->socketDescriptor()] = YapConnection(socket);
    connect(socket, &QTcpSocket::readyRead, this, &YapTcpServer::readSocket);
    connect(socket, &QTcpSocket::disconnected,this, &YapTcpServer::disconnectSocket);
}

void YapTcpServer::disconnectSocket(){
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    socket->close();
}

YapTcpServer::~YapTcpServer()
{
    mTcpServer->close();
}
