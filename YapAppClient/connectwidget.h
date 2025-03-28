#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class ConnectWidget;
}
QT_END_NAMESPACE

class ConnectWidget : public QWidget
{
    Q_OBJECT

public:
    ConnectWidget(QWidget *parent = nullptr);
    ~ConnectWidget();

private:
    Ui::ConnectWidget *ui;
};
#endif // CONNECTWIDGET_H
