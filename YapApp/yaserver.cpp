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
    if(request.method == "POST")
    {
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
        else if (request.path == "/contacts") {
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
        else if (request.path == "/messages") {
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
        else if (request.path.startsWith("/files"))
        {
            if (json.isObject()) {
                QJsonObject obj = json.object();
                QJsonObject res = db.setFileMeta(
                    obj["sender_id"].toInt(),
                    obj["receiver_id"].toInt(),
                    obj["file_name"].toString()
                    );
                QJsonObject responseObj;

                responseObj["type"] = "file_meta";
                responseObj["success"] = res["success"].toBool();
                if(res["success"].toBool())
                {
                    responseObj["file_path"] = res["file_path"].toString();
                }
                body = QJsonDocument(responseObj).toJson();
            }
            else
            {
                QString baseDir = QCoreApplication::applicationDirPath();
                QString fullPath = baseDir + request.path;

                QFileInfo fileInfo(fullPath);
                QDir dir;

                // Создать все нужные папки (если их нет)
                if (!dir.mkpath(fileInfo.path())) {
                    qWarning() << "Не удалось создать папку:" << fileInfo.path();
                    // Можно вернуть ошибку клиенту
                }
                else {
                    QFile file(fullPath);
                    if (file.open(QIODevice::WriteOnly))
                    {
                        file.write(request.body);
                        file.close();

                        qDebug() << "Файл успешно сохранён в:" << fullPath;
                        // Можно отправить успешный ответ клиенту
                    }
                    else
                    {
                        qWarning() << "Не удалось открыть файл для записи:" << fullPath;
                        // Можно вернуть ошибку клиенту
                    }
                }
            }
        }
    }
    else if(request.method == "GET")
    {
        if (request.path.startsWith("/files"))
        {

            QString baseDir = QCoreApplication::applicationDirPath();
            QString fullPath = baseDir + request.path;

            QFileInfo fileInfo(fullPath);
            QDir dir(fileInfo.path());

            if (dir.exists())
            {
                QFile file(fullPath);
                if(file.open(QIODevice::ReadOnly))
                {
                    body = file.readAll();
                    response.append("HTTP/1.1 200 OK\r\n");
                    response.append("Content-Type: application/octet-stream\r\n");
                    response.append("Content-Length: " + QByteArray::number(body.size()) + "\r\n");
                    response.append("Connection: keep-alive\r\n");
                    response.append(QString("X-Path: %1\r\n\r\n").arg(request.path).toUtf8());
                    response.append(body);
                    socket.write(response);
                    return;
                }
            }
            response.append("HTTP/1.1 404 NotFound\r\n");
            response.append("Content-Length: 0\r\n");
            response.append("Connection: keep-alive\r\n\r\n");
            socket.write(response);
            return;

        }
        else if (request.path == "/contacts") { // GET /contacts?user_id=1
            if (request.queryParams.contains("user_id"))
            {
                QJsonArray contacts = db.getUserContacts(request.queryParams["user_id"].toInt());
                QJsonObject responseObj;
                responseObj["type"] = "contacts";
                responseObj["contacts"] = contacts;
                body = QJsonDocument(responseObj).toJson();
            }
        }
        else if (request.path == "/messages") {// GET /messages?user1_id=1&user2_id=2&limit=100
            if (request.queryParams.contains("user1_id") &&
                request.queryParams.contains("user2_id")) {
                QJsonArray messages = db.getChatMessages(
                    request.queryParams["user1_id"].toInt(),
                    request.queryParams["user2_id"].toInt(),
                    request.queryParams.value("limit", "100").toInt()
                    );
                QJsonObject responseObj;
                responseObj["type"] = "messages";
                responseObj["messages"] = messages;
                body = QJsonDocument(responseObj).toJson();
            }
        }
        else if (request.path == "/users"){
            // GET /users?username=alice
            //       OR
            // GET /users?id=1
            if (request.queryParams.contains("username") ||
                request.queryParams.contains("id")) {

                QJsonObject user;
                if(request.queryParams.contains("username")){
                    user = db.getUserByUsername(request.queryParams["username"]);
                }
                if(request.queryParams.contains("id")){
                    user = db.getUserById(request.queryParams["id"].toInt());
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
