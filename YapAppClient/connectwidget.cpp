#include "connectwidget.h"
#include "client.h"
#include "ui_connectwidget.h"
#include "widgetmanager.h"

ConnectWidget::ConnectWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConnectWidget)
{
    ui->setupUi(this);
    connectBtn = ui->ConnectButton;
    ipLine = ui->IpLine;
    portLine = ui->PortLine;
    errorLabel = ui->ErrorLabel;
    SetErrorMessage("");
}

ConnectWidget::~ConnectWidget()
{
    delete ui;
}

void ConnectWidget::on_ConnectButton_pressed()
{
    int port = 0;
    bool isPort = true;
    port = portLine->text().toInt(&isPort);
    if(isPort)
    {
        qDebug() << "(" <<ipLine->text() <<")" << ":" << port;
        Client::getInstance().Connect(ipLine->text(), port);
        connect(&Client::getInstance(),&Client::OnConnected,this,&ConnectWidget::OnConnect);
        connect(&Client::getInstance(),&Client::OnErrorConnect,this,&ConnectWidget::OnError);
    }
    else
        qDebug() << "Некорректный порт!";
}

void ConnectWidget::OnConnect()
{
    WidgetManager::getInstance().showWidget("auth");
}

void ConnectWidget::OnError()
{
    SetErrorMessage("Error connect");
}

void ConnectWidget::SetErrorMessage(const QString text)
{
    if(text.isEmpty()){
        errorLabel->hide();
        return;
    }
    errorLabel->show();
    errorLabel->setStyleSheet(
        QString("QLabel { color: red;}")
        );
    errorLabel->setText(text);
}

