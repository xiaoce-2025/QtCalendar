#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QCalendarWidget>
#include <QListWidget>
#include <QStringList>
#include <QMap>
#include <QDate>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QPushButton>
#include "customcalendarwidget.h"
#include <QStackedWidget>
#include "listviewwidget.h"

class CalendarWidget : public QWidget {
    Q_OBJECT

public:
    CalendarWidget(QWidget *parent = nullptr);
    // 设置日程数据（传入标题列表）
    void setSchedules(const QMap<QDate, QStringList>& schedules);
    // 设置查看哪一天
    void setDay(const QDate date);
    void clearCache();
    // 强制刷新用（主界面打开或子界面返回时不会主动触发页面刷新）
    void MainWindow_Refresh();
    // 禁止返回
    void mustnotback();

private slots:
    // 日期更改
    void onDateChanged();
    // 计算高度用
    void updateItemHeights();
    // 查看详情
    void onDetailButtonClicked();
    // 返回
    void onBackButtonClicked();

signals:
    void BackRequest();

protected:
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QLabel *eventsTitle;
    CustomCalendarWidget *calendar;
    QListWidget *scheduleList;
    QMap<QDate, QStringList> scheduleMap;  // 日期到事件ID列表
    QMap<QDate, QStringList> scheduleMapTitle;  // 日期到事件标题列表（）
    QMap<QString, QJsonObject> eventRegistry; // ID到事件详情
    QSet<QString> loadedMonths;
    QWidget *rightPanel;
    QPushButton *detailButton;
    QPushButton* backButton;
    QStackedWidget *stackedWidget;
    QWidget *mainCalendarWidget;
    ListViewWidget *listViewWidget;
    void setupUI();
};



#endif // CALENDARWIDGET_H
