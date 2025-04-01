#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

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
    void SetErrorMessage(const QString text);
    ~ConnectWidget();

private slots:
    void on_ConnectButton_pressed();
    void OnConnect();
    void OnError();

private:
    QPushButton* connectBtn;
    QLabel* errorLabel;
    QLineEdit* ipLine;
    QLineEdit* portLine;
    Ui::ConnectWidget *ui;
};
#endif // CONNECTWIDGET_H
