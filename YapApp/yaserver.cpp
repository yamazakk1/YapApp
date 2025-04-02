#include "yaserver.h"
#include "database.h"
#include "yahttpserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

YaServer::YaServer(QObject *parent) : QObject(parent), httpServer(nullptr)
{
    DatabaseManager::instance().connect();
    DatabaseManager::instance().initDatabaseSchema();
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
    qDebug() << "new request";
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
            responseObj["type"] = "register";
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
                obj["email"].toString(),
                obj["password"].toString()
                );

            if (!user.isEmpty()) {
                user["type"] = "login";
                user["token"] = YaHttpServer::generateToken();
                body = QJsonDocument(user).toJson();
            } else {
                QJsonObject error;
                error["type"] = "login";
                error["error"] = "Authentication failed";
                body = QJsonDocument(error).toJson();
            }
        }
    }
    else if (request.path == "/contacts/get") {
        if (json.isObject()) {
            QJsonObject obj = json.object();
            QJsonArray contacts = db.getUserContacts(obj["user_id"].toInt());
            QJsonObject responseObj;
            responseObj["type"] = "contacts";
            responseObj["contacts"] = contacts;
            body = QJsonDocument(responseObj).toJson();
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
            responseObj["type"] = "contacts_add";
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
            responseObj["type"] = "messages_send";
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
            QJsonObject responseObj;
            responseObj["type"] = "messages";
            responseObj["messages"] = messages;
            body = QJsonDocument(responseObj).toJson();
        }
    }
    else if (request.path == "/users/get")
    {
        if (json.isObject()) {
            QJsonObject obj = json.object();

            QJsonObject user;
            if(obj.contains("username")){
                user = db.getUserByUsername(obj["username"].toString());
            }
            if(obj.contains("id")){
                user = db.getUserById(obj["id"].toInt());
            }
            QJsonObject responseObj;
            responseObj["type"] = "user_search";
            if(!user.isEmpty())
            {
                responseObj["success"] = true;
                responseObj["id"] = user.value("id").toInt();
                responseObj["username"] = user.value("username").toString();
                responseObj["email"] = user.value("email").toString();
            }
            else
            {
                responseObj["success"] = false;
            }
            body = QJsonDocument(responseObj).toJson();
        }
    }

    response.append("HTTP/1.1 200 OK\r\n");
    response.append("Content-Type: application/json\r\n");
    response.append("Content-Length: " + QByteArray::number(body.size()) + "\r\n");
    response.append("Connection: keep-alive\r\n\r\n");
    response.append(body);

    socket.write(response);
}

void YaServer::disconnect(QTcpSocket &socket)
{
    socket.deleteLater();
}
