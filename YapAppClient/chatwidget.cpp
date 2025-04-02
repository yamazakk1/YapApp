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
    currentUserLabel = new QLabel("Никнейм");
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

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(messageInput);
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

    connect(sendButton, &QPushButton::clicked, this, &ChatWidget::onSendClicked);
    connect(refreshButton, &QPushButton::clicked, this, &ChatWidget::onRefreshClicked);
    connect(searchButton, &QPushButton::clicked, this, &ChatWidget::onSearchClicked);
    connect(messageInput, &QLineEdit::returnPressed, this, &ChatWidget::onSendClicked);
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
    messageLayout->insertWidget(messageLayout->count() - 1, bubbleWidget);

    //возвращаем прошлое относительное положение
    QTimer::singleShot(1, [bar, scrollRatio]() {
        int newMax = bar->maximum();
        bar->setValue(qRound(scrollRatio * newMax));
    });

}


void ChatWidget::setActiveUser(const QString& username)
{
    currentUserLabel->setText(username);
    chatScrollArea->verticalScrollBar()->setValue(chatScrollArea->verticalScrollBar()->maximum());
}

void ChatWidget::addContact(const QString& name, const QString& lastMessage)
{
    QListWidgetItem* item = new QListWidgetItem(QString("%1\n%2").arg(name, lastMessage));
    contactList->addItem(item);
}

void ChatWidget::onSendClicked()
{
    QString text = messageInput->text().trimmed();
    if (!text.isEmpty()) {
        addMessageBubble(text, true, QTime::currentTime());
        messageInput->clear();
    }
}

void ChatWidget::onRefreshClicked()
{
    qDebug() << "Обновление контактов и сообщений...";
}

void ChatWidget::onSearchClicked()
{
    searchWidget->show();
}
