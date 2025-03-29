#include "authorizationwidget.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QTimer>

AuthorizationWidget::AuthorizationWidget(QWidget *parent)
    : QWidget(parent), isLoginMode(true) {
    setWindowTitle("Авторизация");
    resize(300, 200);

    auto *layout = new QVBoxLayout(this);

    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Имя пользователя");

    emailEdit = new QLineEdit(this);
    emailEdit->setPlaceholderText("Email");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);

    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setPlaceholderText("Подтвердите пароль");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    authButton = new QPushButton(this);
    switchModeButton = new QPushButton(this);
    statusLabel = new QLabel(this);
    statusLabel->setStyleSheet("color: red");

    layout->addWidget(usernameEdit);
    layout->addWidget(emailEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(confirmPasswordEdit);
    layout->addWidget(authButton);
    layout->addWidget(switchModeButton);
    layout->addWidget(statusLabel);

    connect(authButton, &QPushButton::clicked, this, &AuthorizationWidget::handleAuthAction);
    connect(switchModeButton, &QPushButton::clicked, this, &AuthorizationWidget::switchMode);

    updateForm();
}

void AuthorizationWidget::switchMode() {
    isLoginMode = !isLoginMode;
    statusLabel->clear();
    updateForm();
}

void AuthorizationWidget::updateForm() {
    if (isLoginMode) {
        setWindowTitle("Вход");
        authButton->setText("Войти");
        switchModeButton->setText("Создать аккаунт");

        usernameEdit->hide();
        confirmPasswordEdit->hide();
    } else {
        setWindowTitle("Регистрация");
        authButton->setText("Зарегистрироваться");
        switchModeButton->setText("У меня уже есть аккаунт");

        usernameEdit->show();
        confirmPasswordEdit->show();
    }

    QTimer::singleShot(1, [this]() {
        resize(300, 0);
    });
}

void AuthorizationWidget::handleAuthAction() {
    QString email = emailEdit->text().trimmed();
    QString password = passwordEdit->text();
    QStringList errors;

    if (email.isEmpty()) {
        errors << "Email не должен быть пустым.";
    } else {
        QRegularExpression emailRegex(R"((^[\w\.-]+@[\w\.-]+\.\w{2,}$))");
        if (!emailRegex.match(email).hasMatch()) {
            errors << "Некорректный формат email.";
        }
    }

    if (password.isEmpty()) errors << "Пароль не должен быть пустым.";

    if (!isLoginMode) {
        QString username = usernameEdit->text().trimmed();
        QString confirmPassword = confirmPasswordEdit->text();

        if (username.isEmpty()) errors << "Имя пользователя не должно быть пустым.";
        if (confirmPassword.isEmpty()) errors << "Подтвердите пароль.";
        if (password != confirmPassword)
            errors << "Пароли не совпадают.";
    }

    if (!errors.isEmpty()) {
        statusLabel->setStyleSheet("color: red");
        statusLabel->setText(errors.join("\n"));
        return;
    }

    if (isLoginMode) {
        statusLabel->setStyleSheet("color: green");
        statusLabel->setText("Успешный вход!");
    } else {
        statusLabel->setStyleSheet("color: green");
        statusLabel->setText("Регистрация успешна!");
    }
}
