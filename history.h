#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>
#include <QTabWidget>
#include <QListWidget>
#include <QJsonObject>
#include <QJsonArray>
#include "api.h"

class History : public QWidget
{
    Q_OBJECT
public:
    explicit History(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;

private:
    QTabWidget *tabWidget;
    QListWidget *listFinishedEvents;
    QListWidget *listDeletedEvents;

    void loadFinishedEvents();
    void loadDeletedEvents();
    void createEventItem(QListWidget *listWidget, const QJsonObject &event, bool isDeleted = false);
};

#endif // HISTORY_H
