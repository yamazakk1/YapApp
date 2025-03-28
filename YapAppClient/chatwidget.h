#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>

#include<QListWidget>
#include<QPushButton>
#include<QLineEdit>
#include<QScrollArea>
#include<QVBoxLayout>
#include<QSplitter>
#include<QScrollBar>
#include<QTime>
#include <QtMath>
#include<QTimer>
#include<QLabel>


class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);

    void addContact(const QString& name, const QString& lastMessage);
    void setActiveUser(const QString& username);
    void addMessageBubble(const QString& text, bool isOwn, const QTime& time);

private slots:
    void onSendClicked();
    void onRefreshClicked();
    void onSearchClicked();

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
};


#endif // CHATWIDGET_H
