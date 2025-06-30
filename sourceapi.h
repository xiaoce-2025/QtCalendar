#ifndef SOURCEAPI_H
#define SOURCEAPI_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTime>

class SourceAPI
{
public:
    SourceAPI();
    // 检查事件是否合规，合规则返回true
    static bool CheckEvent(QJsonObject MyEvent);
    // 创建事件，true表示成功
    static bool CreateEvent(QJsonObject MyEvent,QString rePath = "/schedules/");
    // 检查事件是否存在，返回值不是空串为存在
    static QString SearchEvent(QString id,QString rePath);
    // 修改事件,true表示成功
    static bool UpdateEvent(QString id,QJsonObject NewEvent,QString rePath = "/schedules/");
    // 查询事件 事件不存在返回空对象
    static QJsonObject RetrieveEvent(QString id,QString rePath = "/schedules/");
    // 查询某一月份事件 月份不存在返回空对象
    static QJsonArray RetrieveMonthEvent(QString year_month,QString rePath = "/schedules/");
    // 查询所有月份 返回列表
    static QStringList RetrieveAllEventMonth(QString rePath = "/schedules/");
    // 删除事件，true为删除成功
    static bool DeleteEvent(QString id,QString rePath = "/schedules/");
    // ...
};

#endif // SOURCEAPI_H
