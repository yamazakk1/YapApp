#include "database.h"
#include <QSettings>
#include <QDebug>
#include <QDateTime>

DatabaseManager* DatabaseManager::m_instance = nullptr;

DatabaseManager& DatabaseManager::instance()
{
    if (!m_instance) {
        m_instance = new DatabaseManager();
    }
    return *m_instance;
}

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QPSQL", "server_db_connection");
    connect();
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::connect() {
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName("127.0.0.1");
    m_db.setPort(5432);
    m_db.setDatabaseName("yapapp");
    m_db.setUserName("postgres");
    m_db.setPassword("123");

    // Добавьте эту строку для отладки
    m_db.setConnectOptions("connect_timeout=5;application_name=MyApp");

    if (!m_db.open()) {
        qDebug() << "Ошибка PostgreSQL:";
        qDebug() << "Код:" << m_db.lastError().nativeErrorCode();
        qDebug() << "Текст:" << m_db.lastError().text();
        return false;
    }
    return true;
}

void DatabaseManager::initDatabaseSchema()
{
    QSqlQuery query(m_db);

    // Users table
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id SERIAL PRIMARY KEY, "
               "username VARCHAR(50) UNIQUE NOT NULL, "
               "password_hash VARCHAR(64) NOT NULL, "
               "email VARCHAR(100) UNIQUE, "
               "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");

    // Messages table
    query.exec("CREATE TABLE IF NOT EXISTS messages ("
               "id SERIAL PRIMARY KEY, "
               "sender_id INTEGER REFERENCES users(id) ON DELETE CASCADE, "
               "receiver_id INTEGER REFERENCES users(id) ON DELETE CASCADE, "
               "content TEXT NOT NULL, "
               "sent_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
               "is_read BOOLEAN DEFAULT FALSE)");

    // Indexes
    query.exec("CREATE INDEX IF NOT EXISTS idx_messages_sender ON messages(sender_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_messages_receiver ON messages(receiver_id)");
}

QString DatabaseManager::hashPassword(const QString &password)
{
    return QCryptographicHash::hash(
               password.toUtf8(),
               QCryptographicHash::Sha256
               ).toHex();
}

bool DatabaseManager::registerUser(const QString &username, const QString &password, const QString &email)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users (username, password_hash, email) "
                  "VALUES (:username, :password, :email)");
    query.bindValue(":username", username);
    query.bindValue(":password", hashPassword(password));
    query.bindValue(":email", email);

    return query.exec();
}

QJsonObject DatabaseManager::authenticateUser(const QString &username, const QString &password)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, username, email FROM users "
                  "WHERE username = :username AND password_hash = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", hashPassword(password));

    if (!query.exec() || !query.next()) {
        return QJsonObject();
    }

    QJsonObject user;
    user["id"] = query.value("id").toInt();
    user["username"] = query.value("username").toString();
    user["email"] = query.value("email").toString();

    return user;
}

bool DatabaseManager::sendMessage(int senderId, int receiverId, const QString &content)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO messages (sender_id, receiver_id, content) "
                  "VALUES (:sender, :receiver, :content)");
    query.bindValue(":sender", senderId);
    query.bindValue(":receiver", receiverId);
    query.bindValue(":content", content);

    return query.exec();
}

QJsonArray DatabaseManager::getUserMessages(int userId, int limit)
{
    QJsonArray messages;
    QSqlQuery query(m_db);
    query.prepare("SELECT m.id, m.content, m.sent_at, m.is_read, "
                  "sender.username as sender_name, "
                  "receiver.username as receiver_name "
                  "FROM messages m "
                  "JOIN users sender ON m.sender_id = sender.id "
                  "JOIN users receiver ON m.receiver_id = receiver.id "
                  "WHERE m.sender_id = :user OR m.receiver_id = :user "
                  "ORDER BY m.sent_at DESC "
                  "LIMIT :limit");
    query.bindValue(":user", userId);
    query.bindValue(":limit", limit);

    if (query.exec()) {
        while (query.next()) {
            QJsonObject msg;
            msg["id"] = query.value("id").toInt();
            msg["content"] = query.value("content").toString();
            msg["sent_at"] = query.value("sent_at").toDateTime().toString(Qt::ISODate);
            msg["is_read"] = query.value("is_read").toBool();
            msg["sender"] = query.value("sender_name").toString();
            msg["receiver"] = query.value("receiver_name").toString();
            messages.append(msg);
        }
    }

    return messages;
}

bool DatabaseManager::markMessageAsRead(int messageId)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE messages SET is_read = TRUE WHERE id = :id");
    query.bindValue(":id", messageId);
    return query.exec();
}

QJsonObject DatabaseManager::getUserById(int userId)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, username, email FROM users WHERE id = :id");
    query.bindValue(":id", userId);

    if (!query.exec() || !query.next()) {
        return QJsonObject();
    }

    QJsonObject user;
    user["id"] = query.value("id").toInt();
    user["username"] = query.value("username").toString();
    user["email"] = query.value("email").toString();

    return user;
}

QJsonObject DatabaseManager::getUserByUsername(const QString &username)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, username, email FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec() || !query.next()) {
        return QJsonObject();
    }

    QJsonObject user;
    user["id"] = query.value("id").toInt();
    user["username"] = query.value("username").toString();
    user["email"] = query.value("email").toString();

    return user;
}

bool DatabaseManager::isConnected() const
{
    return m_db.isOpen();
}
