#include "yaptcpserver.h"
#include "yaserver.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "YapApp_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    //YapTcpServer s;
    YaServer server;
    if(!server.listen(QHostAddress::Any, 33333))
    {
        qDebug() << "server is not started";
    }
    else
    {
        qDebug() << "server is started";
    }
    return a.exec();
}
