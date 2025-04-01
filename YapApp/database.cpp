#include "database.h"
#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>
DatabaseManager* DatabaseManager::m_instance = nullptr;

DatabaseManager& DatabaseManager::instance() {
    if (!m_instance) {
        m_instance = new DatabaseManager();
    }
    return *m_instance;
}

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::connect() {
    m_db = QSqlDatabase::addDatabase("QPSQL", "yapapp");
    if (m_db.isOpen())
        return true;
    // Основные параметры
    m_db.setHostName("localhost"); // Явно указываем IPv4
    m_db.setPort(5488);
    m_db.setDatabaseName("yapapp");
    m_db.setUserName("postgres"); // Или yapapp_user
    m_db.setPassword("321"); // Ваш пароль

    // Важные опции
    m_db.setConnectOptions(
        "connect_timeout=5;"
        "application_name=YapApp;"
        "requiressl=0" // Если SSL не настроен
        );

    if (!m_db.open()) {
        qDebug() << "Ошибка PostgreSQL:";
        qDebug() << "Код:" << m_db.lastError().nativeErrorCode();
        qDebug() << "Текст:" << m_db.lastError().text();
        return false;
    }

    qDebug() << "Успешное подключение к PostgreSQL!";
    return true;
}

void DatabaseManager::initDatabaseSchema() {
    QSqlQuery query(m_db);

    // Users table
            query.exec("CREATE TABLE IF NOT EXISTS users ("
                       "id SERIAL PRIMARY KEY, "
                       "username TEXT UNIQUE NOT NULL, "
                       "password_hash TEXT NOT NULL, "
                       "email TEXT UNIQUE NOT NULL"
                       ")");

    // Messages table
    query.exec("CREATE TABLE IF NOT EXISTS messages ("
               "id SERIAL PRIMARY KEY, "
               "sender_id INTEGER REFERENCES users(id) ON DELETE CASCADE, "
               "receiver_id INTEGER REFERENCES users(id) ON DELETE CASCADE, "
               "content TEXT NOT NULL, "
               "sent_at TIMESTAMP DEFAULT NOW(), "
               "is_read BOOLEAN DEFAULT FALSE"
               ")");
    // Contacts table
    query.exec("CREATE TABLE IF NOT EXISTS contacts ("
               "user_id INTEGER REFERENCES users(id) ON DELETE CASCADE, "
               "contact_id INTEGER REFERENCES users(id) ON DELETE CASCADE, "
               "PRIMARY KEY (user_id, contact_id))");

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
QJsonArray DatabaseManager::getUserContacts(int userId) {
    QJsonArray contacts;
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT u.id, u.username, "
        "(SELECT content FROM messages "
        "WHERE (sender_id = :user AND receiver_id = u.id) OR "
        "(sender_id = u.id AND receiver_id = :user) "
        "ORDER BY sent_at DESC LIMIT 1) AS last_message "
        "FROM users u "
        "JOIN contacts c ON u.id = c.contact_id "
        "WHERE c.user_id = :user"
        );
    query.bindValue(":user", userId);

    if (query.exec()) {
        while (query.next()) {
            QJsonObject contact;
            contact["id"] = query.value("id").toInt();
            contact["name"] = query.value("username").toString();
            contact["last_message"] = query.value("last_message").toString();
            contacts.append(contact);
        }
    }
    return contacts;
}

bool DatabaseManager::addContact(int userId, int contactId) {
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO contacts (user_id, contact_id) VALUES (:user, :contact)");
    query.bindValue(":user", userId);
    query.bindValue(":contact", contactId);
    return query.exec();
}

QJsonArray DatabaseManager::getChatMessages(int user1, int user2, int limit) {
    QJsonArray messages;
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT content, sender_id, sent_at "
        "FROM messages "
        "WHERE (sender_id = :user1 AND receiver_id = :user2) OR "
        "(sender_id = :user2 AND receiver_id = :user1) "
        "ORDER BY sent_at DESC "
        "LIMIT :limit"
        );
    query.bindValue(":user1", user1);
    query.bindValue(":user2", user2);
    query.bindValue(":limit", limit);

    if (query.exec()) {
        while (query.next()) {
            QJsonObject msg;
            msg["text"] = query.value("content").toString();
            msg["is_my"] = (query.value("sender_id").toInt() == user1);
            msg["time"] = query.value("sent_at").toDateTime().toString("HH:mm");
            messages.append(msg);
        }
    }
    return messages;
}
