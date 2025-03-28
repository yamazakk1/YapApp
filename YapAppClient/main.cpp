#include "connectwidget.h"
#include "chatwidget.h"
#include "widgetmanager.h"

#include <QApplication>
#include <QStackedWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WidgetManager::getInstance().addWidget("connect", new ConnectWidget());
    WidgetManager::getInstance().addWidget("chat", new ChatWidget());
    WidgetManager::getInstance().showWidget("connect");
    return a.exec();
}
