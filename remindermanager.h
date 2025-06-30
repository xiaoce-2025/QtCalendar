#ifndef REMINDERMANAGER_H
#define REMINDERMANAGER_H

#include <QObject>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QSet>
#include "api.h" // 引用你提供的日程数据接口

class ReminderManager : public QObject {
    Q_OBJECT

public:
    explicit ReminderManager(QObject *parent = nullptr);
    void startMonitoring(); // 启动提醒监控

private slots:
    void checkReminders(); // 检查待提醒事件
    void showReminder(const QString &title, const QString &eventId); // 显示提醒弹窗

private:
    QTimer *reminderTimer;          // 定时检查定时器
    QSystemTrayIcon *trayIcon;      // 系统托盘图标
    QSet<QString> triggeredEvents; // 已触发提醒的事件ID集合，避免重复提醒
};

#endif // REMINDERMANAGER_H
