#include "httpresponse.h"

QString HttpResponse::header(const QString &name) const
{
    return headers.value(name.toLower(), "");
}

bool HttpResponse::isValid() const
{
    return valid;
}

bool HttpResponse::isJson() const
{
    QString contentType = header("content-type").toLower();
    return contentType.contains("application/json");
}

QJsonDocument HttpResponse::json() const
{
    if (!isJson())
        return QJsonDocument();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(body, &err);
    return (err.error == QJsonParseError::NoError) ? doc : QJsonDocument();
}

HttpResponse HttpResponse::parse(const QByteArray &rawResponse)
{
    HttpResponse result;

    // Разделение заголовков и тела
    int headerEnd = rawResponse.indexOf("\r\n\r\n");
    if (headerEnd == -1) {
        result.valid = false;
        return result;
    }

    QByteArray headerData = rawResponse.left(headerEnd);
    result.body = rawResponse.mid(headerEnd + 4);

    QList<QByteArray> lines = headerData.split('\n');
    if (lines.isEmpty()) {
        result.valid = false;
        return result;
    }

    // Первая строка: HTTP/1.1 200 OK
    QString statusLine = QString(lines[0]).trimmed();
    QStringList statusParts = statusLine.split(' ');
    if (statusParts.size() < 3 || !statusLine.startsWith("HTTP/")) {
        result.valid = false;
        return result;
    }

    result.statusCode = statusParts[1].toInt();
    result.statusText = statusParts.mid(2).join(" ");

    // Заголовки
    for (int i = 1; i < lines.size(); ++i) {
        QString line = QString(lines[i]).trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(':', Qt::KeepEmptyParts);
        if (parts.size() >= 2) {
            QString key = parts[0].toLower();
            QString value = parts.mid(1).join(":").trimmed();
            result.headers[key] = value;
        }
    }

    result.valid = true;
    return result;
}
