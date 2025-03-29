#include "yaserver.h"
#include "database.h"
#include "yahttpserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

YaServer::YaServer(QObject *parent) : QObject(parent), httpServer(nullptr)
{
    DatabaseManager::instance();
}

YaServer::~YaServer()
{
    if (httpServer != nullptr) {
        httpServer->close();
        delete httpServer;
    }
}

YaServer& YaServer::getInstance()
{
    static YaServer instance;
    return instance;
}

void YaServer::startServer(int port)
{
    if (httpServer != nullptr) {
        httpServer->close();
        delete httpServer;
    }

    httpServer = new YaHttpServer(this);

    if (!httpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Server failed to start:" << httpServer->errorString();
        return;
    }

    connect(httpServer, &YaHttpServer::newRequest, this, &YaServer::newRequest);
    connect(httpServer, &YaHttpServer::disconnect, this, &YaServer::disconnect);

    qDebug() << "Server started on port:" << port;
}

void YaServer::newRequest(QTcpSocket &socket, HttpRequest &request)
{
    QJsonDocument json;
    if (request.header("Content-Type") == "application/json") {
        json = QJsonDocument::fromJson(request.body);
    }

    QByteArray response;
    QByteArray body = "{}";
    DatabaseManager& db = DatabaseManager::instance();

    if (request.path == "/register") {
        if (json.isObject()) {
            QJsonObject obj = json.object();
            bool success = db.registerUser(
                obj["username"].toString(),
                obj["password"].toString(),
                obj["email"].toString()
                );

            QJsonObject responseObj;
            responseObj["success"] = success;
            if (!success) {
                responseObj["error"] = "Registration failed";
            }
            body = QJsonDocument(responseObj).toJson();
        }
    }
    else if (request.path == "/login") {
        if (json.isObject()) {
            QJsonObject obj = json.object();
            QJsonObject user = db.authenticateUser(
                obj["username"].toString(),
                obj["password"].toString()
                );

            if (!user.isEmpty()) {
                user["token"] = YaHttpServer::generateToken();
                body = QJsonDocument(user).toJson();
            } else {
                QJsonObject error;
                error["error"] = "Authentication failed";
                body = QJsonDocument(error).toJson();
            }
        }
    }
    else if (request.path == "/contacts/get") {
        if (json.isObject()) {
            QJsonObject obj = json.object();
            QJsonArray contacts = db.getUserContacts(obj["user_id"].toInt());
            body = QJsonDocument(contacts).toJson();
        }
    }
    else if (request.path == "/contacts/add") {
        if (json.isObject()) {
            QJsonObject obj = json.object();
            bool success = db.addContact(
                obj["user_id"].toInt(),
                obj["contact_id"].toInt()
                );

            QJsonObject responseObj;
            responseObj["success"] = success;
            body = QJsonDocument(responseObj).toJson();
        }
    }
    else if (request.path == "/messages/send") {
        if (json.isObject()) {
            QJsonObject obj = json.object();
            bool success = db.sendMessage(
                obj["sender_id"].toInt(),
                obj["receiver_id"].toInt(),
                obj["content"].toString()
                );
            QJsonObject responseObj;
            responseObj["success"] = success;
            body = QJsonDocument(responseObj).toJson();
        }
    }
    else if (request.path == "/messages/get") {
        if (json.isObject()) {
            QJsonObject obj = json.object();
            QJsonArray messages = db.getChatMessages(
                obj["user1_id"].toInt(),
                obj["user2_id"].toInt(),
                obj.value("limit").toInt(100)
                );
            body = QJsonDocument(messages).toJson();
        }
    }

    response.append("HTTP/1.1 200 OK\r\n");
    response.append("Content-Type: application/json\r\n");
    response.append("Content-Length: " + QByteArray::number(body.size()) + "\r\n");
    response.append("Connection: close\r\n\r\n");
    response.append(body);

    socket.write(response);
    socket.disconnectFromHost();
}

void YaServer::disconnect(QTcpSocket &socket)
{
    socket.deleteLater();
}
