#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileInfo>
#include <QDir>

class Client: public QObject
{
    Q_OBJECT
public:
    static Client& getInstance();
    void startServer(int port);

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    void Connect(QString ip, int port);
    void SendHttp(const QString metodeName, const QString url, const QJsonObject* json = nullptr) const;
    void SendFile(const QString metodeName, const QString url, const QByteArray* file) const;
    void OpenServerFile(const QString url);
    QString username;
    QString token;
    int userId = -1;
signals:
    void OnConnected();
    void OnErrorConnect();
    void OnLoginSuccess(QJsonObject user);
    void OnLoginError(QString error);
    void OnRegisterSuccess();
    void OnRegisterError(QString error);
    void OnUserSearchSuccess(QJsonObject user);
    void OnUserSearchError(QString error);
    void OnContactsReceived(QJsonArray contacts);
    void OnContactAdded();
    void OnContactAddError(QString error);
    void OnMessageSent();
    void OnMessagesReceived(QJsonArray messages);
    void OnFileMetaReceived(QString filePathServer);
    void OnFileMetaError();
    void OnErrorResponse(QString message);
private:
    QList<QString> filesToOpen;
    QTcpSocket* m_socket;
    QString m_ip;
    int m_port;
    ~Client();
    Client();
private slots:
    void onErrorResponse(QString message);
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onErrorSocketConnect();
};

#endif // CLIENT_H
