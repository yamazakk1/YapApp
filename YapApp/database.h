#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QCoreApplication>
class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    static DatabaseManager& instance();

    bool connect();
    bool isConnected() const;

    // ДЕлишки с пользователями
    bool registerUser(const QString &username, const QString &password, const QString &email = "");
    QJsonObject authenticateUser(const QString &username, const QString &password);
    QJsonObject getUserById(int userId);
    QJsonObject getUserByUsername(const QString &username);

    // Делишки с сообщениями
    bool sendMessage(int senderId, int receiverId, const QString &content);
    QJsonArray getUserMessages(int userId, int limit = 50);
    bool markMessageAsRead(int messageId);

    void initDatabaseSchema();
    QJsonArray getUserContacts(int userId);
    bool addContact(int userId, int contactId);
    QJsonArray getChatMessages(int user1, int user2, int limit = 100);
private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();


    QString hashPassword(const QString &password);

    QSqlDatabase m_db;
    static DatabaseManager* m_instance;
};

#endif // DATABASE_H
