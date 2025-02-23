#ifndef YAPTCPSERVER_H
#define YAPTCPSERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>
#include "user.h"

class YapTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit YapTcpServer(QObject *parent = nullptr);
    ~YapTcpServer();
    void registerSoket(QTcpSocket *socket);
    QString getSocketName(QTcpSocket *socket);
    struct YapConnection
    {
        public:
        QTcpSocket *socket;
        User *user;
        YapConnection(QTcpSocket *socket = nullptr) : socket(socket), user(nullptr){}
    };
public slots:
    void connectSocket();
    void disconnectSocket();
    void readSocket();
private:
    QTcpServer* mTcpServer;
    QMap<long long, YapConnection> mSockets;
// socket->setProperty("", 1)
// socket->property("clientId").toInt();
    YapConnection& getConnection(QTcpSocket *socket);
    QString ReadQString(QTcpSocket *socket);
    void registUser(QTcpSocket *socketm, QString message = "");
};
#endif // YAPTCPSERVER_H
