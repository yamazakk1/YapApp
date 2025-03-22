#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QString>
#include <QMap>

class HttpRequest
{
public:
    QString method;
    QString path;
    QMap<QString, QString> headers;
    QMap<QString, QString> queryParams;
    QByteArray body;
    bool valid = true;

    QString header(const QString& name) const;
    QString query(const QString& name) const;
    bool isValid() const;

    static HttpRequest parse(const QByteArray& rawRequest);
};
#endif // HTTPREQUEST_H
