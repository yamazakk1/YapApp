#include "connectwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConnectWidget w;
    w.show();
    return a.exec();
}
