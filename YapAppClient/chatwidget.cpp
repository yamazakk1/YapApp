#include "chatwidget.h"

#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QSplitter>

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
{
    searchWidget = new SearchContactWidget(parent);
    // ----- Контакты -----
    contactList = new QListWidget();
    refreshButton = new QPushButton("Обновить");
    searchButton = new QPushButton("Поиск 🔍");

    QVBoxLayout* contactLayout = new QVBoxLayout();
    contactLayout->addWidget(searchButton);
    contactLayout->addWidget(contactList);
    contactLayout->addWidget(refreshButton);

    QWidget* contactPanel = new QWidget();
    contactPanel->setLayout(contactLayout);
    contactPanel->setMinimumWidth(150);
    contactPanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);


    // ----- Никнейм сверху -----
    currentUserLabel = new QLabel("Выберите чат");
    currentUserLabel->setStyleSheet("font-weight: bold; font-size: 16px; padding: 4px;");
    currentUserLabel->setAlignment(Qt::AlignCenter);

    // ----- Чат -----
    messageContainer = new QWidget();
    messageContainer->setContentsMargins(0, 0, 0, 0);
    messageLayout = new QVBoxLayout(messageContainer);

    messageLayout->addStretch();
    messageLayout->setContentsMargins(0, 0, 0, 0);
    messageLayout->setSpacing(1);



    chatScrollArea = new QScrollArea();
    chatScrollArea->setWidgetResizable(true);
    chatScrollArea->setWidget(messageContainer);
    chatScrollArea->setContentsMargins(0, 0, 0, 0);


    messageInput = new QLineEdit();
    sendButton = new QPushButton("Отправить");
    sendFileButton = new QPushButton("Отправить файл");

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendFileButton);
    inputLayout->addWidget(sendButton);

    QVBoxLayout* chatLayout = new QVBoxLayout();
    chatLayout->addWidget(currentUserLabel);
    chatLayout->addWidget(chatScrollArea);
    chatLayout->addLayout(inputLayout);

    QWidget* chatPanel = new QWidget();
    chatPanel->setLayout(chatLayout);
    chatPanel->setMinimumSize(300, 500);
    chatPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);




    // ----- Сплиттер -----
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(contactPanel);
    splitter->addWidget(chatPanel);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);
    splitter->setHandleWidth(0);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(splitter);
    setLayout(mainLayout);
    setMinimumSize(700, 500);

    connect(sendButton, &QPushButton::clicked,
            this, &ChatWidget::onSendClicked);
    connect(sendFileButton, &QPushButton::clicked,
            this, &ChatWidget::onSendFileClicked);
    connect(refreshButton, &QPushButton::clicked,
            this, &ChatWidget::onRefreshClicked);
    connect(searchButton, &QPushButton::clicked,
            this, &ChatWidget::onSearchClicked);
    connect(messageInput, &QLineEdit::returnPressed,
            this, &ChatWidget::onSendClicked);
    connect(&Client::getInstance(), &Client::OnContactAdded,
            this, &ChatWidget::onContactAdded);
    connect(&Client::getInstance(), &Client::OnContactsReceived,
            this, &ChatWidget::onContactsReceived);
    connect(&Client::getInstance(), &Client::OnMessagesReceived,
            this, &ChatWidget::onMessagesReceived);
    connect(&Client::getInstance(), &Client::OnFileMetaReceived,
            this, &ChatWidget::onFileMetaReceived);
    connect(&Client::getInstance(), &Client::OnFileMetaError,
            this, &ChatWidget::onFileMetaError);
    connect(contactList, &QListWidget::itemClicked,
            this, &ChatWidget::onContactClicked);

    QTimer* refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &ChatWidget::onRefreshClicked);
    refreshTimer->start(100000);
}

void ChatWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event); // обязательно вызываем базовый метод
    onRefreshClicked();        // вызываем нужную функцию
}

void ChatWidget::addMessageBubble(const QString& text, bool isOwn, const QTime& time)
{
    // Основной текст
    QLabel* textLabel = new QLabel(text);
    textLabel->setWordWrap(true);
    textLabel->setStyleSheet(
        QString("QLabel { background-color: %1; color: black; border-radius: 10px; padding: 8px; }")
            .arg(isOwn ? "#d1f7c4" : "#f0f0f0")
        );

    // Время
    QLabel* timeLabel = new QLabel(time.toString("hh:mm"));
    timeLabel->setStyleSheet("color: gray; font-size: 10px;");
    timeLabel->setAlignment(isOwn ? Qt::AlignRight : Qt::AlignLeft);

    // Объединяем текст и время вертикально
    QVBoxLayout* bubbleTextLayout = new QVBoxLayout();
    bubbleTextLayout->addWidget(textLabel);
    bubbleTextLayout->addWidget(timeLabel);

    QWidget* bubbleInner = new QWidget();
    bubbleInner->setLayout(bubbleTextLayout);

    // Горизонтальный layout для выравнивания левого/правого
    QHBoxLayout* bubbleLayout = new QHBoxLayout();
    bubbleLayout->setContentsMargins(0, 0, 0, 0);
    bubbleLayout->setSpacing(0);
    if (isOwn) {
        bubbleLayout->addStretch();
        bubbleLayout->addWidget(bubbleInner);
    } else {
        bubbleLayout->addWidget(bubbleInner);
        bubbleLayout->addStretch();
    }

    // Высчитывам положение скролла
    QScrollBar* bar = chatScrollArea->verticalScrollBar();

    int scrollMax = bar->maximum();
    int scrollVal = bar->value();

    double scrollRatio = scrollMax > 0 ? static_cast<double>(scrollVal) / scrollMax : 1.0;

    // Добавляем виджет сообшенияf

    QWidget* bubbleWidget = new QWidget();
    bubbleWidget->setLayout(bubbleLayout);
    messageLayout->insertWidget(messageLayout->count()-1, bubbleWidget);

    //возвращаем прошлое относительное положение
    QTimer::singleShot(1, [bar, scrollRatio]() {
        int newMax = bar->maximum();
        bar->setValue(qRound(scrollRatio * newMax));
    });

}

void ChatWidget::addFileBubble(const QString& filePath , bool isOwn, const QTime& time)
{
    // Основной текст
    QLabel* fileIcon = new QLabel();
    QPixmap pixmap(":/images/file_icon.png");
    if (pixmap.isNull()) {
        qDebug() << "Не удалось загрузить картинку!";
    }
    fileIcon->setScaledContents(true);
    fileIcon->setPixmap(pixmap);
    fileIcon->setStyleSheet(
        QString("QLabel { background-color: %1; color: black; border-radius: 10px; padding: 8px; }")
            .arg(isOwn ? "#d1f7c4" : "#f0f0f0")
        );
    fileIcon->setFixedSize(60, 60);

    // Время
    QLabel* timeLabel = new QLabel(time.toString("hh:mm"));
    timeLabel->setStyleSheet("color: gray; font-size: 10px;");
    timeLabel->setAlignment(isOwn ? Qt::AlignRight : Qt::AlignLeft);

    // Объединяем иконку и время вертикально
    QVBoxLayout* bubbleTextLayout = new QVBoxLayout();
    bubbleTextLayout->addWidget(fileIcon);
    bubbleTextLayout->addWidget(timeLabel);

    QWidget* bubbleInner = new QWidget();
    bubbleInner->setLayout(bubbleTextLayout);

    // Горизонтальный layout для выравнивания левого/правого
    QHBoxLayout* bubbleLayout = new QHBoxLayout();
    bubbleLayout->setContentsMargins(0, 0, 0, 0);
    bubbleLayout->setSpacing(0);
    if (isOwn) {
        bubbleLayout->addStretch();
        bubbleLayout->addWidget(bubbleInner);
    } else {
        bubbleLayout->addWidget(bubbleInner);
        bubbleLayout->addStretch();
    }

    // Высчитывам положение скролла
    QScrollBar* bar = chatScrollArea->verticalScrollBar();

    int scrollMax = bar->maximum();
    int scrollVal = bar->value();

    double scrollRatio = scrollMax > 0 ? static_cast<double>(scrollVal) / scrollMax : 1.0;

    // Добавляем виджет сообшения

    QWidget* bubbleWidget = new QWidget();
    bubbleWidget->setLayout(bubbleLayout);
    messageLayout->insertWidget(messageLayout->count()-1, bubbleWidget);

    //возвращаем прошлое относительное положение
    QTimer::singleShot(1, [bar, scrollRatio]() {
        int newMax = bar->maximum();
        bar->setValue(qRound(scrollRatio * newMax));
    });

}

void ChatWidget::getMessagesFromUser(const int userId)
{
    currentUserIdChat = userId;
    Client::getInstance().SendHttp(
        "GET",
        QString("/messages?user1_id=%1&user2_id=%2")
            .arg(Client::getInstance().userId)
            .arg(userId)
        );
}


void ChatWidget::setActiveUser(const QString& username)
{
    currentUserLabel->setText(username);
    chatScrollArea->verticalScrollBar()->setValue(chatScrollArea->verticalScrollBar()->maximum());
}

void ChatWidget::addContact(const int user_id, const QString& name, const QString& lastMessage)
{
    QListWidgetItem* item = new QListWidgetItem(QString("%1\n%2").arg(name, lastMessage));
    item->setData(Qt::UserRole, user_id);
    item->setData(Qt::UserRole+1, name);
    contactList->addItem(item);
}

void ChatWidget::onSendClicked()
{
    QString text = messageInput->text().trimmed();
    if (!text.isEmpty() && currentUserIdChat != -1) {
        addMessageBubble(text, true, QTime::currentTime());
        QJsonObject obj;
        obj["sender_id"] = Client::getInstance().userId;
        obj["receiver_id"] = currentUserIdChat;
        obj["content"] = text;
        Client::getInstance().SendHttp("POST", "/messages", &obj);
        messageInput->clear();
    }
}

void ChatWidget::onSendFileClicked()
{
    if(currentUserIdChat == -1 || currentSendingFilePath != "") return;
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл");
    if (filePath.isEmpty()) return;
    QFileInfo info(filePath);
    currentSendingFilePath = filePath;
    QJsonObject obj;
    obj["sender_id"] = Client::getInstance().userId;
    obj["receiver_id"] = currentUserIdChat;
    obj["file_name"] = info.fileName().replace(' ', '_');
    Client::getInstance().SendHttp(
        "POST",
        "/files",
        &obj
        );
}


void ChatWidget::onRefreshClicked()
{
    Client::getInstance().SendHttp(
        "GET",
        QString("/contacts?user_id=%1")
            .arg(Client::getInstance().userId)
        );
    if(currentUserIdChat != -1)
    {
        getMessagesFromUser(currentUserIdChat);
    }
}

void ChatWidget::onSearchClicked()
{
    searchWidget->show();
}

void ChatWidget::onContactsReceived(QJsonArray contacts)
{
    contactList->clear();
    for (const QJsonValue &value : contacts) {
        QJsonObject contact = value.toObject();
        QString name = contact["name"].toString();
        QString lastMessage = contact["last_message"].toString();
        if(lastMessage.length() > 14+3)
            lastMessage = lastMessage.first(14) + "...";
        addContact(contact["id"].toInt(), name, lastMessage);
    }
}

void ChatWidget::onContactAdded()
{
    onRefreshClicked();
}

void ChatWidget::onContactClicked(QListWidgetItem *item)
{
    int userId = item->data(Qt::UserRole).toInt();
    QString userName = item->data(Qt::UserRole+1).toString();
    currentUserIdChat = userId;
    currentUserLabel->setText(userName);
    getMessagesFromUser(userId);
}

void clearLayout(QLayout* layout)
{
    if (!layout) return;

    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }

        if (QLayout* childLayout = item->layout()) {
            clearLayout(childLayout);
        }

        delete item;
    }
}

void ChatWidget::onMessagesReceived(QJsonArray messages)
{
    clearLayout(messageLayout);
    messageLayout->addStretch();
    QList<QJsonObject> sortedMessages;

    for (const QJsonValue& val : messages) {
        sortedMessages.append(val.toObject());
    }

    std::reverse(sortedMessages.begin(), sortedMessages.end());

    for (const QJsonObject& msg : sortedMessages) {
        bool isOwn = msg["is_my"].toBool();
        QTime time = QDateTime::fromString(msg["sent_at"].toString(), Qt::ISODate)
                         .toLocalTime().time();
        if(msg["type"].toString() == "text")
        {
            QString text = msg["content"].toString();
            addMessageBubble(text, isOwn, time);
        }
    }

    QScrollBar* scrollBar = chatScrollArea->verticalScrollBar();
    if (scrollBar)
        scrollBar->setValue(scrollBar->maximum());
}

void ChatWidget::onFileMetaReceived(QString filePathServer)
{
    QByteArray bytes;
    QFile file(currentSendingFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        bytes = file.readAll();
        file.close();
        Client::getInstance().SendFile("POST", filePathServer, &bytes);
        addFileBubble(filePathServer, true, QTime::currentTime());
    } else {
        qWarning() << "Не удалось открыть файл для записи: " << currentSendingFilePath;
    }
    currentSendingFilePath = "";
}

void ChatWidget::onFileMetaError()
{
    currentSendingFilePath = "";
}
