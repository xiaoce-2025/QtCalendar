#include "eventwidget.h"
#include "ui_eventwidget.h"
#include <QDir>
#include <QFileDialog>
#include <QCloseEvent>
#include "mymessagebox.h"
#include "sourceapi.h"
#include "api.h"



EventWidget::EventWidget(QJsonObject jsonobj,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EventWidget)
{
    ui->setupUi(this);
    isClosing=false;
    init(jsonobj);
    this->setStyleSheet(R"(
      QWidget {
        font-family: 'Segoe UI';
        font-size: 13px;
        background-color: #f5f7fa;
      }

      /* 分组框样式 */
      QGroupBox {
        border: 1px solid #d3dce6;
        border-radius: 4px;
        margin-top: 20px;
        padding-top: 15px;
      }

      QGroupBox::title {
        subcontrol-origin: margin;
        left: 10px;
        color: #2c3e50;
        font-weight: bold;
      }

      /* 日历 */
      QCalendarWidget {
        background: white;
        border-radius: 4px;
      }

      QCalendarWidget QToolButton {
        color: #409EFF;
        font-size: 14px;
      }

      /* 输入控件 */
      QLineEdit, QTextEdit, QDateTimeEdit, QComboBox {
        border: 1px solid #dcdfe6;
        border-radius: 3px;
        padding: 5px 8px;
        background: white;
      }

      QLineEdit:focus, QTextEdit:focus, QDateTimeEdit:focus {
        border-color: #409EFF;
      }

      /* 复选框 */
      QCheckBox {
        color: #5a5e66;
        spacing: 5px;
      }

      QCheckBox::indicator {
        width: 16px;
        height: 16px;
      }

      /* 按钮 */
      QPushButton {
        background-color: #409EFF;
        color: white;
        border: none;
        border-radius: 4px;
        padding: 8px 15px;
        min-width: 80px;
      }

      QPushButton:hover {
        background-color: #66b1ff;
      }

      QPushButton:pressed {
        background-color: #3a8ee6;
      }

      /* 附件选择按钮 */
      #select_filepath {
        background-color: #67C23A;
      }

      #select_filepath:hover {
        background-color: #85ce61;
      }

      /* 优先级下拉框配色 */
      QComboBox QAbstractItemView {
        background: white;
        selection-background-color: #ecf5ff;
      })");
}


//对外接口部分
bool EventWidget::save(){
    qDebug()<<"尝试保存日程";

    if (ui->edit_EventTitle->text().isEmpty()){
        API::ToastInfo(this,"保存失败：请输入标题",2000);
        return false;
    }

    if (!ui->ifStartTime->isChecked() && !ui->ifWndTime->isChecked()){
        API::ToastInfo(this,"保存失败：请至少输入开始/结束时间中的一项",2000);
        return false;
    }

    if (ui->ifStartTime->isChecked() && date.isValid() && ui->ifWndTime->isChecked() && date.isValid()){
        if (starttime>endtime){
            API::ToastInfo(this,"保存失败：结束时间不能早于开始时间",2000);
            return false;
        }
    }

    QJsonObject obj;

    // 必填字段
    obj["id"] = id;
    obj["title"] = title;

    // 日期时间处理 (ISO8601格式)
    auto formatDateTime = [](const QDateTime& dt) {
        return dt.toString("yyyy-MM-ddTHH:mm");
    };

    // start
    if (ui->ifStartTime->isChecked() && date.isValid()) {
        QDateTime startDt(date, starttime);
        obj["start"] = starttime.isValid() ? formatDateTime(startDt) : "NULL";
    } else {
        obj["start"] = "NULL";
    }

    // end
    if (ui->ifWndTime->isChecked() && date.isValid()){
        QDateTime endDt(date, endtime);
        obj["end"] = endtime.isValid() ? formatDateTime(endDt) : "NULL";
    } else {
        obj["end"] = "NULL";
    }

    // 剩余字段处理
    if (ui->IfCategory->isChecked() &&!category.isEmpty()) {
        obj["category"] = category;
    }
    if (ui->Ifpriority->isChecked() && priority!=-114) {
        obj["priority"] = priority;
    }

    // 提醒
    if (ui->IfReminder->isChecked() && remindertime.isValid()) {
        QJsonArray reminders;
        QJsonObject rem;
        rem["type"] = "notification";
        rem["trigger"] = "-15m";
        rem["time"] = formatDateTime(remindertime);
        reminders.append(rem);
        obj["reminders"] = reminders;
    }

    // 附件处理
    QJsonArray attachments;
    if (ui->IfAtURL->isChecked() && !attachurl.isEmpty()) {
        QJsonObject att;
        att["type"] = "url";
        att["link"] = attachurl;
        attachments.append(att);
    }
    if (ui->IfAtDoc->isChecked() && !attachpath.isEmpty()) {
        QJsonObject att;
        att["type"] = "file";
        att["path"] = attachpath;
        attachments.append(att);
    }
    if (!attachments.isEmpty()) {
        obj["attachments"] = attachments;
    }

    // 描述
    if (ui->IfDesp->isChecked() && !ui->edit_desp->toPlainText().isEmpty()){
        obj["description"]=ui->edit_desp->toPlainText();
    }

    if (eventFrom.isEmpty()){
        eventFrom = "手动添加";
    }
    obj["from"] = eventFrom;

    //事件保存/修改接口
    if (IsRevise){
        qDebug()<<"开始修改日程";
        if(!SourceAPI::UpdateEvent(id,obj)){
            MyMessageBox::critical(this, "错误", "事件修改失败");
            return false;
        }
        else{
            qDebug()<<"success";
            return true;
        }
    }
    else{
        qDebug()<<"开始创建日程";
        if(!SourceAPI::CreateEvent(obj)){
            MyMessageBox::critical(this, "错误", "创建事件失败");
            return false;
        }
        else{
            IsRevise=true;
            qDebug()<<"success";
            return true;
        }
    }
    return false;
}

void EventWidget::init(QJsonObject obj){
    if (obj.isEmpty()){
        IsRevise = false;
        qDebug()<<"从当前日期初始化日程";
        setDate(QDate::currentDate());
        setStartTime(QTime::currentTime());
        setEndTime(QTime::currentTime());
        setReminderTime(QDateTime::currentDateTime());
        id = QDateTime::currentDateTime().toString("yyMMddHHmmsszzz");
        ui->ifStartTime->setChecked(true);
        ui->Ifpriority->setChecked(true);
        ui->edit_priority->setCurrentIndex(2);
        ui->ifWndTime->setChecked(true);
        ui->IfReminder->setChecked(true);
        eventFrom = "手动添加";
    }
    else{
        IsRevise = true;
        id = obj["id"].toString();
        qDebug()<<"从"<<id<<"初始化日程";

        if (obj.contains("title")) {
            setTitle(obj["title"].toString());
        }

        // 日期处理（优先使用start的日期）
        QDate date;
        if (obj.contains("start") && !obj["start"].isNull()) {
            ui->ifStartTime->setChecked(true);
            QDateTime startDt = QDateTime::fromString(obj["start"].toString(), Qt::ISODate);
            date = startDt.date();
            setDate(date);
            setStartTime(startDt.time());
        }
        else{
            ui->ifStartTime->setChecked(false);
        }
        if (obj.contains("end")) {
            ui->ifWndTime->setChecked(true);
            QDateTime endDt = QDateTime::fromString(obj["end"].toString(), Qt::ISODate);
            // 若start日期无效则用end的日期
            if (!date.isValid()) {
                setDate(endDt.date());
            }
            setEndTime(endDt.time());
        }
        else{
            ui->ifWndTime->setChecked(false);
        }

        if (obj.contains("category")) {
            setCategory(obj["category"].toString());
            ui->IfCategory->setChecked(true);
        }
        else{
            ui->IfCategory->setChecked(false);
        }
        if (obj.contains("priority")) {
            setPriority(obj["priority"].toInt());
            ui->Ifpriority->setChecked(true);
        }
        else{
            ui->Ifpriority->setChecked(false);
        }

        // 提醒处理（取第一个包含time的提醒）
        if (obj.contains("reminders")) {
            QJsonArray reminders = obj["reminders"].toArray();
            for (const auto& r : reminders) {
                QJsonObject rem = r.toObject();
                if (rem.contains("time")) {
                    setReminderTime(QDateTime::fromString(rem["time"].toString(), Qt::ISODate));
                    break;
                }
            }
            ui->IfReminder->setChecked(true);
        }
        else{
            ui->IfReminder->setChecked(false);
        }

        // 附件处理
        if (obj.contains("attachments")) {
            QJsonArray attachments = obj["attachments"].toArray();
            for (const auto& a : attachments) {
                QJsonObject att = a.toObject();
                if (att["type"] == "url") {
                    setAttachUrl(att["link"].toString());
                } else if (att["type"] == "file") {
                    setAttachPath(att["path"].toString());
                }
            }
        }

        if (obj.contains("description")) {
            ui->edit_desp->setPlainText(obj["description"].toString());
            ui->IfDesp->setChecked(true);
        }
        else{
            ui->IfDesp->setChecked(false);
        }
        if (obj.contains("from")){
            eventFrom = obj["from"].toString();
        }
        else{
            eventFrom = "旧版日程，来源未知";
        }
    }
}


//自我接口部分
void EventWidget::setDate(const QDate d){
    date=d;
    qDebug()<<"设置日期"<<date;
    ui->calendarWidget->setSelectedDate(date);
    ui->dateEdit->setDate(date);

}
void EventWidget::setTitle(const QString str){
    qDebug()<<"设置标题:"<<str;
    ui->edit_EventTitle->setText(str);
    title=str;
}
void EventWidget::setStartTime(const QTime time){
    qDebug()<<"设置开始时间:"<<time;
    ui->edit_start_time->setTime(time);
    starttime=time;
}
void EventWidget::setEndTime(const QTime time){
    qDebug()<<"设置结束时间:"<<time;
    ui->edit_end_time->setTime(time);
    endtime=time;
}
void EventWidget::setCategory(const QString str){
    qDebug()<<"设置事件种类:"<<str;
    ui->edit_category->setText(str);
    category=str;
}
void EventWidget::setPriority(const int n){
    qDebug()<<"设置事件优先级:"<<n;
    ui->edit_priority->setCurrentIndex(n);
    priority=n;
}
void EventWidget::setReminderTime(const QDateTime datetime){
    qDebug()<<"设置提醒时间:"<<datetime;
    ui->edit_remind_time->setDateTime(datetime);
    remindertime=datetime;
}
void EventWidget::setAttachUrl(const QString str){
    qDebug()<<"设置附加链接:"<<str;
    ui->edit_attachUrl->setText(str);
    attachurl=str;
}
void EventWidget::setAttachPath(const QString str){
    qDebug()<<"设置附加路径:"<<str;
    ui->edit_attachPath->setText(str);
    attachpath=str;
}


EventWidget::~EventWidget()
{
    delete ui;
}



void EventWidget::closeEvent(QCloseEvent *event){
    if (!isClosing){
        MyMessageBox choose(nullptr,tr("请确认"),tr("当前事件编辑尚未保存"),QMessageBox::Question,QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        choose.button(QMessageBox::Yes)->setText(tr("不保存"));
        choose.button(QMessageBox::No)->setText(tr("保存并退出"));
        choose.button(QMessageBox::Cancel)->setText(tr("返回编辑"));
        int temp_15=choose.exec();
        if (temp_15 == QMessageBox::Yes){
            qDebug()<<"取消修改";
        }
        else if (temp_15 == QMessageBox::No){
            if(!save()){
                event->ignore();
                return;
            }
            else{
                event->accept();
                return;
            }
        }
        else{
            event->ignore();
        }
    }
}

void EventWidget::on_calendarWidget_clicked(const QDate &date)
{
    setDate(date);
}


void EventWidget::on_confirm_clicked()
{
    if(!save()){
        return;
    }
    isClosing=true;
    Q_EMIT finish(true);
    close();
}


void EventWidget::on_cancel_clicked()
{
    MyMessageBox choose(nullptr,tr("请确认"),tr("当前事件编辑尚未保存"),QMessageBox::Question,QMessageBox::Yes | QMessageBox::No);
    choose.button(QMessageBox::Yes)->setText(tr("不保存"));
    choose.button(QMessageBox::No)->setText(tr("返回编辑"));
    int temp_15=choose.exec();
    if (temp_15 == QMessageBox::Yes){
        qDebug()<<"取消修改";
        isClosing=true;
        Q_EMIT finish(false);
        close();
    }
}


void EventWidget::on_dateEdit_dateChanged(const QDate &date)
{
    setDate(date);
}


void EventWidget::on_edit_EventTitle_textChanged(const QString &arg1)
{
    setTitle(arg1);
}


void EventWidget::on_edit_start_time_timeChanged(const QTime &time)
{
    setStartTime(time);
}


void EventWidget::on_edit_end_time_timeChanged(const QTime &time)
{
    setEndTime(time);
}


void EventWidget::on_edit_category_textChanged(const QString &arg1)
{
    setCategory(arg1);
    if (arg1.isEmpty()){
        if (ui->IfCategory->isChecked()){
            ui->IfCategory->setChecked(false);
        }
    }
    else if (!ui->IfCategory->isChecked()){
        ui->IfCategory->setChecked(true);
    }
}


void EventWidget::on_edit_priority_currentIndexChanged(int index)
{
    setPriority(index);
}


void EventWidget::on_edit_remind_time_dateTimeChanged(const QDateTime &dateTime)
{
    setReminderTime(dateTime);
}


void EventWidget::on_edit_attachUrl_textChanged(const QString &arg1)
{
    setAttachUrl(arg1);
    if (ui->edit_attachUrl->text().isEmpty()){
        if (ui->IfAtURL->isChecked()){
            ui->IfAtURL->setChecked(false);
        }
    }
    else if (!ui->IfAtURL->isChecked()){
        ui->IfAtURL->setChecked(true);
    }
}


void EventWidget::on_edit_attachPath_textChanged(const QString &arg1)
{
    setAttachPath(arg1);
    if (ui->edit_attachPath->text().isEmpty()){
        if (ui->IfAtDoc->isChecked()){
            ui->IfAtDoc->setChecked(false);
        }
    }
    else if (!ui->IfAtDoc->isChecked()){
        ui->IfAtDoc->setChecked(true);
    }
}


void EventWidget::on_select_filepath_clicked()
{
    QString curPath = QDir :: currentPath();
    QString dlgTitle = "选择一个附加文件" ;
    QString attachfile_path = QFileDialog ::getOpenFileName(this,dlgTitle,curPath);
    if (!attachfile_path.isEmpty()){
        setAttachPath(attachfile_path);
    }
}


void EventWidget::on_save_clicked()
{
    save();
}


void EventWidget::on_edit_desp_textChanged()
{
    if (ui->edit_desp->toPlainText().isEmpty()){
        if (ui->IfDesp->isChecked()){
            ui->IfDesp->setChecked(false);
        }
    }
    else if (!ui->IfDesp->isChecked()){
        ui->IfDesp->setChecked(true);
    }
}


void EventWidget::on_ifStartTime_clicked(bool checked)
{
    if (checked){
        ui->edit_start_time->show();
    }
    else{
        ui->edit_start_time->hide();
    }
}


void EventWidget::on_ifWndTime_clicked(bool checked)
{
    if (checked){
        ui->edit_end_time->show();
    }
    else{
        ui->edit_end_time->hide();
    }
}


void EventWidget::on_Ifpriority_clicked(bool checked)
{
    if (checked){
        ui->edit_priority->show();
    }
    else{
        ui->edit_priority->hide();
    }
}


void EventWidget::on_IfReminder_clicked(bool checked)
{
    if (checked){
        ui->edit_remind_time->show();
    }
    else{
        ui->edit_remind_time->hide();
    }
}


void EventWidget::setDay(QDate day){
    setDate(day);
}


