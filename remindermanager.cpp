#include "remindermanager.h"
#include "mymessagebox.h"
#include "SingleEventShow.h"
#include "basicvariable.h"

ReminderManager::ReminderManager(QObject *parent) : QObject(parent) {

    // 初始化系统托盘图标（需在资源文件中添加图标）
    trayIcon = new QSystemTrayIcon(QIcon(":/test.ico.png"), this);
    if (!trayIcon->isVisible()) {
        qDebug() << "系统托盘图标显示失败";
    }
    trayIcon->setToolTip("日程提醒");
    trayIcon->show();


    // 创建定时器：每分钟检查一次提醒（可调整为更短间隔）
    reminderTimer = new QTimer(this);
    connect(reminderTimer, &QTimer::timeout, this, &ReminderManager::checkReminders);
}

void ReminderManager::startMonitoring() {
    reminderTimer->start(1000); // 每分钟触发检查
    checkReminders(); // 立即执行首次检查
}

void ReminderManager::checkReminders() {
    QDateTime currentTime = QDateTime::currentDateTime();
    QJsonArray events = API::RetrieveAllEvent(); // 从你提供的SourceAPI获取所有事件

    foreach (const QJsonValue &eventValue, events) {
        QJsonObject event = eventValue.toObject();
        QString eventId = event["id"].toString();

        // 分别记录15分钟提醒和1分钟提醒的状态
        bool reminded15Minutes = triggeredEvents.contains(eventId + "_15m");
        bool reminded1Minute = triggeredEvents.contains(eventId + "_1m");

        // 检查事件是否有提醒设置（参考你提供的事件结构）
        if (true) {
            if (event.contains("start") && !event["start"].isNull() && event["start"].toString() != "NULL") {
                QDateTime startTime = QDateTime::fromString(event["start"].toString(), Qt::ISODate);
                int secsToStart = currentTime.secsTo(startTime);

                // 15分钟前提醒
                if (!reminded15Minutes && (secsToStart >= 15 * 60 - 5 && secsToStart <= 15 * 60 + 5)) {
                    showReminder(event["title"].toString(), eventId);
                    triggeredEvents.insert(eventId + "_15m"); // 标记15分钟提醒
                }

                // 1分钟前提醒
                if (!reminded1Minute && (secsToStart >= 55 && secsToStart <= 65)) {
                    showReminder(event["title"].toString(), eventId);
                    triggeredEvents.insert(eventId + "_1m"); // 标记1分钟提醒
                }
            }
        }
        // 跳过已提醒的事件
        if (triggeredEvents.contains(eventId)) continue;
        if (event.contains("reminders")) {
            QJsonArray reminders = event["reminders"].toArray();

            foreach (const QJsonValue &reminderValue, reminders) {
                QJsonObject reminder = reminderValue.toObject();
                if (!reminder.contains("time")) continue; // 确保提醒时间存在

                // 解析提醒时间（格式需与存储一致，使用 yyyy-MM-ddTHH:mm 格式）
                QDateTime remindTime = QDateTime::fromString(
                    reminder["time"].toString(),
                    "yyyy-MM-ddTHH:mm"
                    );

                // 允许一定的误差范围，比如5秒
                if (currentTime.secsTo(remindTime) >= -5 && currentTime.secsTo(remindTime) <= 5) {
                    showReminder(event["title"].toString(), eventId);
                    triggeredEvents.insert(eventId); // 标记为已提醒
                    break; // 单个事件只需提醒一次
                }
            }
        }
    }
}
void ReminderManager::showReminder(const QString &title, const QString &eventId) {
    // 显示系统托盘提醒（macOS/Windows/Linux通用）
    trayIcon->showMessage(
        "⏰ 日程提醒",
        QString("即将开始：%1\n点击查看详情").arg(title),
        QSystemTrayIcon::Information, // 图标类型
        10 * 1000 // 弹窗显示10秒
        );

    // 点击弹窗跳转到事件详情（需实现对应的界面逻辑）
    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, [eventId]() {
        // 从 SourceAPI 获取事件详情
        QJsonObject event = SourceAPI::RetrieveEvent(eventId);
        if (!event.isEmpty()) {
            // 创建并显示 SingleEventShowWidget
            SingleEventShowWidget *detailWidget = new SingleEventShowWidget(event);
            detailWidget->deleteBtn->hide();
            detailWidget->editBtn->hide();
            detailWidget->finishBtn->hide();
            detailWidget->show();
        }
    });
}
