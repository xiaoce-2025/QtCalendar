#ifndef LISTVIEWWIDGET_H
#define LISTVIEWWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStringList>
#include <QJsonArray>
#include "api.h"
#include "SingleEventShow.h"

class ListViewWidget : public QWidget {
    Q_OBJECT

public:
    ListViewWidget(QWidget *parent = nullptr);
    // 设置日程列表（传入标题列表）
    void setSchedules(const QStringList& schedules);
    // 设置日程列表（传入事件列表）
    void setEvents(const QJsonArray &events);
    // 强制刷新用
    void refreshData();
    void clearDetailView();
    void replaceDetailWidget(QWidget *newWidget);
    // 排序规则
    enum CustomRoles {
        TimeRole = Qt::UserRole + 1
    };
    void clearAllCache();
    // 设置查询事件的函数和标题信息
    void setRange(std::function<QJsonArray()> f, QDate date);

    // 禁止返回
    void mustnotback();

signals:
    void backRequested();

private slots:
    void onItemClicked(QListWidgetItem *item);
    void handleDeleteRequest(const QString &eventId);
    void handleEditRequest(const QJsonObject &event);
    void handleFinishRequest(const QJsonObject &event);
    void handleEditFinished(bool success);
    void onAddButtonClicked();
    void onBackButtonClicked();

private:
    QListWidget *scheduleListWidget;
    QJsonArray m_events;
    void setupUI();
    QWidget *m_detailContainer;
    SingleEventShowWidget *m_currentDetail = nullptr;
    void setListEnabled(bool enabled); //左侧列表是否可点
    QWidget *leftWidget;
    QLabel* eventsTitle;
    QPushButton *addButton;//添加事件
    QPushButton *backButton;//返回
    QLabel* tip;
    QDate sday;
    // 查询事件的函数
    std::function<QJsonArray()> SearchMethod;


public:
    ~ListViewWidget();


};




#endif // LISTVIEWWIDGET_H
