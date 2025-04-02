#include "yahttpserver.h"
#include "httprequest.h"

YaHttpServer::YaHttpServer(QObject *parent) : QTcpServer(parent)
{
}

void YaHttpServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, [socket, this]()
    {
        static QByteArray buffer;
        buffer += socket->readAll();

        while (true) {
            int headerEndIndex = buffer.indexOf("\r\n\r\n");
            if (headerEndIndex == -1)
                return; // ждём остальную часть заголовков

            QByteArray headersPart = buffer.left(headerEndIndex);
            QList<QByteArray> lines = headersPart.split('\n');
            if (lines.isEmpty())
                return;

            // Получаем Content-Length
            int contentLength = 0;
            for (const QByteArray& line : lines) {
                if (line.toLower().startsWith("content-length:")) {
                    contentLength = line.mid(QString("content-length:").length()).trimmed().toInt();
                }
            }

            int totalRequestLength = headerEndIndex + 4 + contentLength;
            if (buffer.size() < totalRequestLength)
                return; // ждём остальную часть body

            QByteArray oneRequest = buffer.left(totalRequestLength);
            buffer = buffer.mid(totalRequestLength); // удаляем обработанную часть

            // Парсим запрос
            HttpRequest res = HttpRequest::parse(oneRequest);
            qDebug()
                << "method: " << res.method << "\n"
                << "path: " << res.path << "\n"
                << "quary: " << res.queryParams << "\n"
                << "headers: " << res.headers << "\n"
                << "body: " << res.body << "\n";

            emit newRequest(*socket, res);

            // если остались данные — цикл обработает следующий
        }
    });
    connect(socket, &QTcpSocket::disconnected, [socket, this]()
    {
        emit disconnect(*socket);
    });
    connect(socket, &QTcpSocket::errorOccurred, this, [](QAbstractSocket::SocketError err) {
        qDebug() << "Ошибка сокета:" << err;
    });
}

QString YaHttpServer::generateToken()
{
     return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
