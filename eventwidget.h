#ifndef EVENTWIDGET_H
#define EVENTWIDGET_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class EventWidget;
}

class EventWidget : public QWidget
{
    Q_OBJECT

public:
    void setDay(QDate day);
    EventWidget(const QJsonObject event = QJsonObject() , QWidget *parent = nullptr);
    ~EventWidget();

private slots:
    void on_calendarWidget_clicked(const QDate &date);

    void on_confirm_clicked();

    void on_cancel_clicked();

    void on_dateEdit_dateChanged(const QDate &date);

    void on_edit_EventTitle_textChanged(const QString &arg1);

    void on_edit_start_time_timeChanged(const QTime &time);

    void on_edit_end_time_timeChanged(const QTime &time);

    void on_edit_category_textChanged(const QString &arg1);

    void on_edit_priority_currentIndexChanged(int index);

    void on_edit_remind_time_dateTimeChanged(const QDateTime &dateTime);

    void on_edit_attachUrl_textChanged(const QString &arg1);

    void on_edit_attachPath_textChanged(const QString &arg1);

    void on_select_filepath_clicked();

    void on_save_clicked();

    void on_edit_desp_textChanged();

    void on_ifStartTime_clicked(bool checked);

    void on_ifWndTime_clicked(bool checked);

    void on_Ifpriority_clicked(bool checked);

    void on_IfReminder_clicked(bool checked);

private:
    Ui::EventWidget *ui;
    void setDate(const QDate date);
    void setTitle(const QString str);
    void setStartTime(const QTime time);
    void setEndTime(const QTime time);
    void setCategory(const QString str);
    void setPriority(const int n);
    void setReminderTime(const QDateTime datetime);
    void setAttachUrl(const QString str);
    void setAttachPath(const QString str);
    bool save();
    bool isClosing=false;

    bool IsRevise;// 是否是查询已有事件

    QString id;
    QString title,category,attachurl,attachpath;
    QTime starttime,endtime;
    QDate date;
    QDateTime remindertime;
    QString eventFrom;
    int priority=0;

signals:
    void finish(bool success);

public:
    void init(QJsonObject obj);

protected:
    void closeEvent(QCloseEvent *);
};

#endif // EVENTWIDGET_H
