#include "yaserver.h"
#include "yahttpserver.h"
#include "database.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlDatabase>
#include <QTimer>

int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    YaServer::getInstance().startServer(8888);
    return a.exec();
}
