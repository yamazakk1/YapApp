#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QObject>

class Client: public QObject
{
    Q_OBJECT
public:
    static Client& getInstance();
    void startServer(int port);

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    void Connect(QString ip, int port);
signals:
    void OnConnected();
    void OnErrorConnect();
private:
    QTcpSocket* socket;
    ~Client();
    Client();
private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError();
};

#endif // CLIENT_H
