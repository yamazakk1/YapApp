#ifndef SEARCHCONTACTWIDGET_H
#define SEARCHCONTACTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "client.h"

class SearchContactWidget : public QWidget
{
     Q_OBJECT
public:
    explicit SearchContactWidget(QWidget *parent = nullptr);
protected:
    void showEvent(QShowEvent* event) override;
private slots:
    void onSearchClicked();
    void onAddContactClicked();
    void onSearchSuccess(QJsonObject json);
    void onSearchError(QString error);
    void onContactAdded();
    void onContactAddError(QString error);

private:
    QLineEdit* searchInput;
    QLabel* emailUser;
    QLabel* usernameUser;
    QPushButton* searchButton;
    QPushButton* addContactButton;
    QWidget* foundUserContainer;
    QLabel*  errorLabel;
    int foundUserId;
};

#endif // SEARCHCONTACTWIDGET_H
