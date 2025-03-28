#ifndef WIDGETMANAGER_H
#define WIDGETMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QWidget>

class WidgetManager : public QObject
{
    Q_OBJECT
public:
    static WidgetManager& getInstance();

    void addWidget(const QString& name, QWidget* widget);
    void showWidget(const QString& name);
    QWidget* getWidget(const QString& name) const;

    WidgetManager(const WidgetManager&) = delete;
    WidgetManager& operator=(const WidgetManager&) = delete;

private:
    explicit WidgetManager(QObject *parent = nullptr);
    ~WidgetManager();

    QMap<QString, QWidget*> widgets;
    QWidget* current = nullptr;
};

#endif // WIDGETMANAGER_H
