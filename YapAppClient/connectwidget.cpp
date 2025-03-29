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
        Client::getInstance().Connect(ipLine->text(), port);
        connect(&Client::getInstance(),&Client::OnConnected,this,&ConnectWidget::OnConnect);
    }
    else
        qDebug() << "Некорректный порт!";
}

void ConnectWidget::OnConnect()
{
    WidgetManager::getInstance().showWidget("chat");
}

void ConnectWidget::OnError()
{

}

