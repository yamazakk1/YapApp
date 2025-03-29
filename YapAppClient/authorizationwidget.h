#ifndef AUTHORIZATIONWIDGET_H
#define AUTHORIZATIONWIDGET_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QLabel;

class AuthorizationWidget : public QWidget {
    Q_OBJECT

public:
    AuthorizationWidget(QWidget *parent = nullptr);

private slots:
    void handleAuthAction();
    void switchMode();

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
