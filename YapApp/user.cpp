#include "user.h"

User::User() {}

User::User(QString firstName, QString lastName, QString email)
{
    setFirstName(firstName);
    setLastName(lastName);
    setEmail(email);
}

QString User::getFirstName() const { return mFirstName; }

QString User::getLastName() const { return mLastName; }

QString User::getEmail() const { return mEmail; }

void User::setFirstName(const QString value) { mFirstName = value; }

void User::setLastName(const QString value) { mLastName = value; }

void User::setEmail(const QString value) { mEmail = value; }
