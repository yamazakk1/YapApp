#include "yaserver.h"
#include "yahttpserver.h"
#include "database.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSqlDatabase>
#include <QDebug>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    YaServer::getInstance().startServer(33333);

    return a.exec();

}
