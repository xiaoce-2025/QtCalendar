#include "api.h"

API::API() {}
// 查询所有事件
QJsonArray API::RetrieveAllEvent()
{
    QJsonArray result;

    // 获取所有包含事件的月份
    const QStringList allMonths = SourceAPI::RetrieveAllEventMonth();
    if (allMonths.isEmpty())
    {
        qDebug() << "ERROR#1101 无月份记录";
        return result;
    }

    foreach (const QString &month, allMonths)
    {
        const QJsonArray monthlyEvents = SourceAPI::RetrieveMonthEvent(month);

        // 查询事件合并
        for (const auto &event : monthlyEvents)
        {
            if (event.isObject() && !event.toObject().isEmpty())
            {
                result.append(event);
            }
        }
    }

    if (result.isEmpty())
    {
        qDebug() << "ERROR#1102 存在月份记录但无有效事件";
    }
    return result;
}

QJsonArray API::RetrieveAllPathEvent(QString rePath)
{
    QJsonArray result;

    // 获取所有包含事件的月份
    const QStringList allMonths = SourceAPI::RetrieveAllEventMonth(rePath);
    if (allMonths.isEmpty())
    {
        qDebug() << "ERROR#1101 无月份记录";
        return result;
    }

    foreach (const QString &month, allMonths)
    {
        const QJsonArray monthlyEvents = SourceAPI::RetrieveMonthEvent(month,rePath);

        // 查询事件合并
        for (const auto &event : monthlyEvents)
        {
            if (event.isObject() && !event.toObject().isEmpty())
            {
                result.append(event);
            }
        }
    }

    if (result.isEmpty())
    {
        qDebug() << "ERROR#1102 存在月份记录但无有效事件";
    }
    return result;
}

// 查询事件日期
QString API::formatEventDate(const QJsonObject &event)
{
    QString dateStr = event["start"].toString();
    if (dateStr == "NULL" || dateStr.isEmpty())
    {
        dateStr = event["end"].toString();
    }

    if (!dateStr.isEmpty())
    {
        QDateTime dt = QDateTime::fromString(dateStr, Qt::ISODate);
        if (dt.isValid())
        {
            return dt.toString("yy-MM-dd");
        }
    }
    return "19-05-05";
}

// 事件合法性查询
bool API::IfEventLegal(QJsonObject MyEvent)
{
    return true;
}

// 完成事件
bool API::FinishEvent(QJsonObject MyEvent)
{
    qDebug() << "事件已完成！";
    SourceAPI::CreateEvent(MyEvent,"/schedules/finished/");
    if (SourceAPI::DeleteEvent(MyEvent["id"].toString())){
        return true;
    }
    return false;
}

// 生成查询某一天事件的函数
std::function<QJsonArray()> API::RetrieveFromDay(const QDate &today)
{
    auto x = [today]() -> QJsonArray
    {
        QJsonArray all = SourceAPI::RetrieveMonthEvent(today.toString("yyyy-MM"));
        QJsonArray list;
        for (const QJsonValue val : all)
        {
            QJsonObject x = val.toObject();
            if (formatEventDate(x) == today.toString("yy-MM-dd"))
            {
                list.append(x);
            }
        }
        return list;
    };
    return x;
}

// 删除事件
bool API::DeleteEvent(QString id){
    QJsonObject event = SourceAPI::RetrieveEvent(id);
    SourceAPI::CreateEvent(event,"/schedules/deleted/");
    if (SourceAPI::DeleteEvent(id)){
        return true;
    }
    return false;
}

// 恢复删除事件
bool API::RevokeDeletedEvent(QString id){
    QJsonObject event = SourceAPI::RetrieveEvent(id,"/schedules/deleted/");
    SourceAPI::CreateEvent(event);
    if (SourceAPI::DeleteEvent(id,"/schedules/deleted/")){
        return true;
    }
    return false;
}

// 删除删除的事件
bool API::DeleteDeletedEvent(QString id){
    if (SourceAPI::DeleteEvent(id,"/schedules/deleted/")){
        return true;
    }
    return false;
}

// 查询所有已完成事件，如不存在则返回空对象
QJsonArray API::RetrieveAllFinishedEvent(){
    return RetrieveAllPathEvent("/schedules/finished/");
}
// 查询所有已删除事件，如不存在则返回空对象
QJsonArray API::RetrieveAllDeletedEvent(){
    return RetrieveAllPathEvent("/schedules/deleted/");
}

