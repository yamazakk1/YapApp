#ifndef AUTHORIZATIONWIDGET_H
#define AUTHORIZATIONWIDGET_H

#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>

class QLineEdit;
class QPushButton;
class QLabel;

class AuthorizationWidget : public QWidget {
    Q_OBJECT

public:
    AuthorizationWidget(QWidget *parent = nullptr);

private slots:
    void handleButtonAuthAction();
    void switchMode();
    void onRegisterSuccess();
    void onLoginSuccess(QJsonObject user);
private:
    void updateForm();

    QLineEdit *usernameEdit;
    QLineEdit *emailEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;

    QPushButton *authButton;
    QPushButton *switchModeButton;

    QLabel *statusLabel;

    bool isLoginMode;
};

#endif // AUTHORIZATIONWIDGET_H
