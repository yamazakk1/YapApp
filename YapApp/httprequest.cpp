#include "httprequest.h"

QString HttpRequest::header(const QString &name) const
{
    return headers.value(name.toLower(), "");
}

QString HttpRequest::query(const QString &name) const
{
    return queryParams.value(name, "");
}

bool HttpRequest::isValid() const
{
    return valid;
}

HttpRequest HttpRequest::parse(const QByteArray &rawRequest)
{
    HttpRequest result;
    QList<QByteArray> lines = rawRequest.split('\n');
    if(lines.isEmpty())
    {
        result.valid = false;
        return result;
    }
    QList<QByteArray> firstLine = rawRequest.split(' ');
    if(firstLine.size() < 2)
    {
        result.valid = false;
        return result;
    }
    result.method = QString(firstLine[0]).trimmed();
    QString fullPath = QString(firstLine[1]).trimmed();
    result.path = fullPath;

    auto quaryIndex = fullPath.indexOf('?');
    if(quaryIndex != -1)
    {
        result.path = fullPath.left(quaryIndex);
        QString quaryPath = fullPath.mid(quaryIndex + 1);
        for (const QString& quaryPair : quaryPath.split('&'))
        {
            QStringList parts = quaryPair.split('=');
            if(parts.size() == 2 && !parts[0].isEmpty() && !parts[1].isEmpty())
                result.queryParams[parts[0]] = parts[1];
        }
    }

    int i = 1;
    for (; i < lines.size(); ++i)
    {
        QString line = QString(lines[i]).trimmed();
        if(line.isEmpty())
            break;
        QStringList parts = line.split(':');
        if(parts.size() == 2 && !parts[0].isEmpty() && !parts[1].isEmpty())
            result.headers[parts[0].toLower()] = parts[1].trimmed();
    }

    QByteArray body;
    for (int j = i + 1; j < lines.size(); ++j) {
        body += lines[j] + '\n';
    }
    result.body = body.trimmed();

    return result;
}
