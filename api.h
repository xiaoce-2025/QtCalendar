#ifndef API_H
#define API_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTime>
#include <sourceapi.h>
#include <qcolor.h>
#include "ToastWidget.h"

class API
{
public:
    API();
    // 事件冲突校验,true为有时间冲突
    static bool IsEventConfliction(QDateTime StartTime,QDateTime EndTime);
    // 添加事件，返回值含义待定
    static int AddEvent(QJsonObject MyEvent);
    // 查询所有事件，如不存在则返回空对象
    static QJsonArray RetrieveAllEvent();
    // 查询所有事件（此函数不应直接调用）
    static QJsonArray RetrieveAllPathEvent(QString rePath = "/schedules/");
    // 事件合法性校验
    static bool IfEventLegal(QJsonObject MyEvent);
    // 完成事件
    static bool FinishEvent(QJsonObject MyEvent);
    // 返回事件日期25-02-02
    static QString formatEventDate(const QJsonObject &event);
    // 返回查询某一日事件的函数（注意这里返回的是查询函数，不是查询值！）
    static std::function<QJsonArray()> RetrieveFromDay(const QDate& today);
    //static QJsonArray(*RetriveFromDay(const QDate& today))();
    // 删除事件
    static bool DeleteEvent(QString id);
    // 恢复删除事件
    static bool RevokeDeletedEvent(QString id);
    // 删除删除的事件
    static bool DeleteDeletedEvent(QString id);
    // 查询所有已完成事件，如不存在则返回空对象
    static QJsonArray RetrieveAllFinishedEvent();
    // 查询所有已删除事件，如不存在则返回空对象
    static QJsonArray RetrieveAllDeletedEvent();

    // 悬浮弹窗提示信息
    template<typename T>
    static void ToastInfo(T parent,QString info,int time){
        ToastWidget* re = new ToastWidget(parent, time);
        re->setText(info);
        re->show();
        return ;
    }
    // 优先级颜色获取
    static QColor getPriorityColor(int priority) {
        static QMap<int, QColor> priorityColors = {
            {0, QColor(255, 0, 0)},
            {1, QColor(217, 64, 0)},
            {2, QColor(179, 128, 0)},
            {3, QColor(140, 191, 0)},
            {4, QColor(102, 255, 0)}
        };
        /*
        生成的色阶：
        #FF0000
        #D94000
        #B38000
        #8CBF00
        #66FF00
        */
        return priorityColors.value(priority, Qt::black);
    };

    // 重要性汉字获取
    static QString change_priority(const int i){
        if (i==0)   return "非常重要";
        else if (i==1)   return "重要";
        else if (i==2)   return "一般";
        else if (i==3)   return "不重要";
        else if (i==4)   return "非常不重要";
        return "";
    }

};

#endif // API_H
