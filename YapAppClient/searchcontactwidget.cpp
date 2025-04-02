#include "searchcontactwidget.h"

SearchContactWidget::SearchContactWidget(QWidget *parent)
    : QWidget(parent)
{
    searchInput = new QLineEdit();
    searchButton = new QPushButton("Поиск 🔍");
    addContactButton = new QPushButton("Добавить контакт");
    emailUser = new QLabel("почта");
    usernameUser = new QLabel("имя");

    errorLabel = new QLabel("ошибка");
    errorLabel->setStyleSheet("color: red;");
    QLabel* emailLabel = new QLabel("Почта:");
    QLabel* usernameLabel = new QLabel("Имя:");
    QVBoxLayout* emailLayout = new QVBoxLayout();
    emailLayout->addWidget(emailLabel);
    emailLayout->addWidget(emailUser);
    QVBoxLayout* usernameLayout = new QVBoxLayout();
    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addWidget(usernameUser);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(searchButton);
    buttonsLayout->addWidget(addContactButton);

    QHBoxLayout* foundUserLayout = new QHBoxLayout();
    foundUserLayout->addLayout(emailLayout);
    foundUserLayout->addLayout(usernameLayout);
    foundUserContainer = new QWidget();
    foundUserContainer->setLayout(foundUserLayout);


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(searchInput);
    mainLayout->addWidget(errorLabel);
    mainLayout->addWidget(foundUserContainer);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);
    setMinimumSize(270, 200);
    setMaximumSize(270, 220);

    foundUserContainer->hide();
    addContactButton->hide();
    errorLabel->hide();

    connect(searchButton, &QPushButton::clicked,
            this, &SearchContactWidget::onSearchClicked);
    connect(addContactButton, &QPushButton::clicked,
            this, &SearchContactWidget::onAddContactClicked);
    connect(&Client::getInstance(), &Client::OnUserSearchSuccess,
            this, &SearchContactWidget::onSearchSuccess);
    connect(&Client::getInstance(), &Client::OnUserSearchError,
            this, &SearchContactWidget::onSearchError);
    connect(&Client::getInstance(), &Client::OnContactAdded,
            this, &SearchContactWidget::onContactAdded);
    connect(&Client::getInstance(), &Client::OnContactAddError,
            this, &SearchContactWidget::onContactAddError);
}

void SearchContactWidget::showEvent(QShowEvent *event)
{
    foundUserContainer->hide();
    addContactButton->hide();
    errorLabel->hide();
}

void SearchContactWidget::onSearchClicked()
{
    foundUserContainer->hide();
    addContactButton->hide();
    errorLabel->hide();
    QJsonObject obj;
    obj["username"] = searchInput->text();
    Client::getInstance().SendHttp("POST", "/users/get", obj);
}

void SearchContactWidget::onAddContactClicked()
{
    QJsonObject obj;
    obj["user_id"] = Client::getInstance().userId;
    obj["contact_id"] = foundUserId;
    Client::getInstance().SendHttp("POST", "/contacts/add", obj);
}

void SearchContactWidget::onSearchSuccess(QJsonObject json)
{
    foundUserContainer->show();
    errorLabel->hide();
    addContactButton->show();
    foundUserId = json["id"].toInt();
    if(foundUserId == Client::getInstance().userId){
        onSearchError("Нельзя добавить самого себя");
        return;
    }
    usernameUser->setText(json["username"].toString());
    emailUser->setText(json["email"].toString());
}

void SearchContactWidget::onSearchError(QString error)
{
    foundUserContainer->hide();
    errorLabel->show();
    addContactButton->hide();
    errorLabel->setText(error);
}

void SearchContactWidget::onContactAdded()
{
    hide();
}

void SearchContactWidget::onContactAddError(QString error)
{
    foundUserContainer->hide();
    errorLabel->show();
    addContactButton->hide();
    errorLabel->setText(error);
}
