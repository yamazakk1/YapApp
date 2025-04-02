#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>

#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollBar>
#include <QTime>
#include <QtMath>
#include <QTimer>
#include <QLabel>
#include <searchcontactwidget.h>


class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);

    void addContact(const int user_id, const QString& name, const QString& lastMessage);
    void setActiveUser(const QString& username);
    void addMessageBubble(const QString& text, bool isOwn, const QTime& time);
    void getMessagesFromUser(const int userId);
protected:
    void showEvent(QShowEvent* event) override;
private slots:
    void onSendClicked();
    void onRefreshClicked();
    void onSearchClicked();
    void onContactsReceived(QJsonArray contacts);
    void onContactAdded();
    void onContactClicked(QListWidgetItem* item);
    void onMessagesReceived(QJsonArray messages);
private:
    QListWidget* contactList;
    QPushButton* searchButton;
    QPushButton* refreshButton;

    QLabel* currentUserLabel;

    QWidget* messageContainer;
    QVBoxLayout* messageLayout;
    QScrollArea* chatScrollArea;

    QLineEdit* messageInput;
    QPushButton* sendButton;
    SearchContactWidget* searchWidget;
    int currentUserIdChat = -1;
};


#endif // CHATWIDGET_H
