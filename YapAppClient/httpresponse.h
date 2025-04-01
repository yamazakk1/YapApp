#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QString>
#include <QMap>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class HttpResponse
{
public:
    int statusCode = 0;
    QString statusText;
    QMap<QString, QString> headers;
    QByteArray body;
    bool valid = true;

    QString header(const QString& name) const;
    bool isValid() const;
    bool isJson() const;
    QJsonDocument json() const;

    static HttpResponse parse(const QByteArray& rawResponse);
};

#endif // HTTPRESPONSE_H
