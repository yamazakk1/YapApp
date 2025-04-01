#include "widgetmanager.h"
#include <QDebug>

WidgetManager& WidgetManager::getInstance()
{
    static WidgetManager instance;
    return instance;
}

WidgetManager::WidgetManager(QObject *parent)
    : QObject(parent)
{
}

WidgetManager::~WidgetManager()
{
}

void WidgetManager::addWidget(const QString& name, QWidget* widget)
{
    if (!widgets.contains(name)) {
        widgets.insert(name, widget);
        widget->hide(); // Сначала всё скрыто
    } else {
        qWarning() << "Widget already exists:" << name;
    }
}

void WidgetManager::showWidget(const QString& name)
{
    if (!widgets.contains(name)) {
        qWarning() << "No such widget:" << name;
        return;
    }

    if (current && current != widgets[name])
        current->hide();

    current = widgets[name];
    current->show();
    current->raise();
}

QWidget* WidgetManager::getWidget(const QString& name) const
{
    return widgets.value(name, nullptr);
}

QWidget* WidgetManager::getCurrent() const
{
    return current;
}
