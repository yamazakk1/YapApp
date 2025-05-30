#include "client.h"
#include "widgetmanager.h"
#include "connectwidget.h"
#include "httpresponse.h"


Client &Client::getInstance()
{
    static Client instance;
    return instance;
}

void Client::Connect(QString ip, int port)
{
    m_socket->connectToHost(ip, port);
    m_ip = ip;
    m_port = port;
}

void Client::SendHttp(const QString metodeName, const QString url, const QJsonObject* json) const
{
    QString urlNoSpace = url;
    urlNoSpace = urlNoSpace.replace(' ', '_');
    QString message = QString("%1 %2 HTTP/1.1\r\nHost: %3\r\n").arg(metodeName, urlNoSpace, m_ip);
    message.append("Connection: keep-alive\r\n");

    if (json && !json->isEmpty()) {
        message.append("Content-Type: application/json\r\n");
        QJsonDocument doc(*json);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        message.append(QString("Content-Length: %1\r\n").arg(jsonData.size()));
        message.append("\r\n");
        message.append(QString::fromUtf8(jsonData));
    }
    else {
        message.append("\r\n");
    }
    m_socket->write(message.toUtf8());
}

void Client::SendFile(const QString metodeName, const QString url, const QByteArray* file) const
{
    QString message = QString("%1 %2 HTTP/1.1\r\nHost: %3\r\n").arg(metodeName, url, m_ip);
    message.append("Connection: keep-alive\r\n");

    if (file && !file->isEmpty()) {
        message.append("Content-Type: application/octet-stream\r\n");
        message.append(QString("Content-Length: %1\r\n").arg(file->size()));
        message.append("\r\n");
        m_socket->write(message.toUtf8());
    }
    else {
        message.append("\r\n");
        m_socket->write(message.toUtf8());
        return;
    }

    m_socket->write(*file);
}

void Client::OpenServerFile(const QString filePath)
{
    QString path = filePath;
    path = path.replace(' ', '_');
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + path;

    QFileInfo fileInfo(fullPath);
    if (fileInfo.exists() && fileInfo.isFile()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));
    } else {
        filesToOpen.append(path);
        SendHttp("GET", path);
    }
}

Client::~Client()
{

}

Client::Client()
{
    m_socket = new QTcpSocket();
    connect(m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &Client::onErrorSocketConnect);
    connect(this, &Client::OnErrorResponse, this, &Client::onErrorResponse);
}

void Client::onErrorResponse(QString message)
{
    qDebug() << "Error response: "<< message;
}

void Client::onConnected()
{
    qDebug() << "Connected to server";
    emit OnConnected();
}

void Client::onReadyRead()
{
    static QByteArray buffer; // сохраняет данные между вызовами

    buffer += m_socket->readAll();

    while (true) {
        int headerEndIndex = buffer.indexOf("\r\n\r\n");
        if (headerEndIndex == -1)
            return; // ждем оставшиеся заголовки

        QByteArray headerPart = buffer.left(headerEndIndex);
        QList<QByteArray> lines = headerPart.split('\n');

        // Получаем Content-Length
        int contentLength = 0;
        for (const QByteArray& line : lines) {
            if (line.toLower().startsWith("content-length:")) {
                contentLength = line.mid(QString("content-length:").length()).trimmed().toInt();
                break;
            }
        }

        int fullLength = headerEndIndex + 4 + contentLength;
        if (buffer.size() < fullLength)
            return; // ждем остальное тело

        QByteArray fullResponse = buffer.left(fullLength);
        buffer = buffer.mid(fullLength); // удалить обработанное

        HttpResponse response = HttpResponse::parse(fullResponse);

        // Проверка корректности ответа
        if (!response.isValid()) {
            emit OnErrorResponse("Некорректный HTTP-ответ от сервера.");
            return;
        }

        // Проверка статуса
        if (response.statusCode != 200) {
            emit OnErrorResponse(QString("Ошибка от сервера: %1 %2")
                             .arg(response.statusCode)
                             .arg(response.statusText));
            return;
        }

        // Проверка, что это JSON
        if (!response.isJson()) {
            QString contentType = response.header("content-type").toLower();
            if(contentType.contains("application/octet-stream"))
            {
                QString path = response.header("x-path");
                QString fullPath = QCoreApplication::applicationDirPath() + path;

                QFileInfo fileInfo(fullPath);
                QDir dir = fileInfo.absoluteDir();
                if (!dir.exists()) {
                    if (!dir.mkpath(".")) {
                        qWarning() << "Не удалось создать директорию:" << dir.absolutePath();
                        return;
                    }
                }

                QFile file(fullPath);
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(response.body);
                    file.close();

                    if(filesToOpen.contains(path)){
                        QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));
                        filesToOpen.removeOne(path);
                    }
                } else {
                    qWarning() << "Не удалось сохранить файл:" << fullPath;
                }

                return;
            }
            emit OnErrorResponse("Неизвестный Content-Type");
            return;
        }

        // Парсим JSON
        QJsonDocument doc = response.json();
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QString type = obj.value("type").toString();

            if (type == "login") {
                if (obj.contains("token"))
                    emit OnLoginSuccess(obj);
                else
                    emit OnLoginError("Ошибка авторизации. Неверный логин или пароль.");
            }
            else if (type == "register") {
                if (obj.value("success").toBool())
                    emit OnRegisterSuccess();
                else
                    emit OnRegisterError("Ошибка регистрации. Email или имя пользователся уже занято.");
            }
            else if (type == "contacts") {
                emit OnContactsReceived(obj.value("contacts").toArray());
            }
            else if (type == "contacts_add") {
                if (obj.value("success").toBool())
                    emit OnContactAdded();
                else
                    emit OnContactAddError("Не удалось добавить контакт.");
            }
            else if (type == "messages") {
                emit OnMessagesReceived(obj.value("messages").toArray());
            }
            else if (type == "messages_send") {
                if (obj.value("success").toBool())
                    emit OnMessageSent();
                else
                    emit OnErrorResponse("Ошибка при отправке сообщения.");
            }
            else if (type == "user_search") {
                if (obj.value("success").toBool())
                    emit OnUserSearchSuccess(obj);
                else
                    emit OnUserSearchError("Пользователь не найден");
            }
            else if (type == "file_meta") {
                if (obj.value("success").toBool())
                    emit OnFileMetaReceived(obj["file_path"].toString());
                else
                    emit OnFileMetaError();
            }
            else {
                emit OnErrorResponse("Неизвестный тип ответа: " + type);
            }
        }
        else {
            emit OnErrorResponse("Неподдерживаемый JSON-ответ.");
        }
    }
}

void Client::onDisconnected()
{
    qDebug() << "Disconected";
    qDebug() << "Try reconect";
    m_socket->connectToHost(m_ip, m_port);
}

void Client::onErrorSocketConnect()
{
    emit OnErrorConnect();
    ConnectWidget* connectWidget = qobject_cast<ConnectWidget*>(WidgetManager::getInstance().getWidget("connect"));
    if(WidgetManager::getInstance().getCurrent() != connectWidget)
    {
        WidgetManager::getInstance().showWidget("connect");
        connectWidget->SetErrorMessage("Error while reconect");
    }
}


