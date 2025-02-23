#ifndef USER_H
#define USER_H

#include <QObject>

class User
{
public:
    User();
    User(QString firstName, QString lastName, QString email);

    QString getFirstName() const;
    QString getLastName() const;
    QString getEmail() const;

    void setFirstName(QString const value);
    void setLastName(QString const value);
    void setEmail(QString const value);
private:
    QString mFirstName;
    QString mLastName;
    QString mEmail;
};
#endif // USER_H
