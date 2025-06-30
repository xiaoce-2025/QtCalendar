#include "history.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include "mymessagebox.h"
#include <QListWidgetItem>
#include <QDateTime>
#include <QDebug>
#include <QString>
#include <QMenu>
#include <QAction>

History::History(QWidget *parent) : QWidget(parent)
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 10, 15, 15);

    // 创建标题
    QLabel *titleLabel = new QLabel(tr("历史记录"));
    titleLabel->setStyleSheet(
        "QLabel {"
        "   font: bold 20px 'Microsoft YaHei';"
        "   color: #2c3e50;"
        "   padding: 10px 0;"
        "   border-bottom: 2px solid #3498db;"
        "}"
        );
    mainLayout->addWidget(titleLabel);

    // 创建选项卡
    tabWidget = new QTabWidget();
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #bdc3c7; border-radius: 8px; background: white; }"
        "QTabBar::tab {"
        "   background: #ecf0f1;"
        "   color: #2c3e50;"
        "   padding: 8px 16px;"
        "   font: 14px 'Microsoft YaHei';"
        "   border: 1px solid #bdc3c7;"
        "   border-bottom: none;"
        "   border-top-left-radius: 5px;"
        "   border-top-right-radius: 5px;"
        "   outline: none;"
        "}"
        "QTabBar::tab:selected {"
        "   background: #3498db;"
        "   color: white;"
        "   font-weight: bold;"
        "}"
        "QTabBar::tab:hover { background: #d6dbdf; }"
        );

    // 已完成事件选项卡
    QWidget *finishedTab = new QWidget;
    QVBoxLayout *finishedLayout = new QVBoxLayout(finishedTab);
    listFinishedEvents = new QListWidget;
    listFinishedEvents->setStyleSheet(
        "QListWidget {"
        "   background: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   border-bottom: 1px solid #ecf0f1;"
        "   padding: 10px;"
        "   outline: none;"
        "}"
        "QListWidget::item:selected {"
        "   background: #e3f2fd;"
        "   color: #2c3e50;"
        "   outline: none;"
        "}"
        );
    finishedLayout->addWidget(listFinishedEvents);
    tabWidget->addTab(finishedTab, tr("已完成事件"));

    // 已删除事件选项卡
    QWidget *deletedTab = new QWidget;
    QVBoxLayout *deletedLayout = new QVBoxLayout(deletedTab);
    listDeletedEvents = new QListWidget;
    listDeletedEvents->setContextMenuPolicy(Qt::CustomContextMenu);
    listDeletedEvents->setStyleSheet(
        "QListWidget {"
        "   background: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   border-bottom: 1px solid #ecf0f1;"
        "   padding: 10px;"
        "   outline: none;"
        "}"
        "QListWidget::item:selected {"
        "   background: #e3f2fd;"
        "   color: #2c3e50;"
        "   outline: none;"
        "}"
        );
    deletedLayout->addWidget(listDeletedEvents);
    tabWidget->addTab(deletedTab, tr("已删除事件"));

    mainLayout->addWidget(tabWidget);

    // 连接上下文菜单信号
    connect(listDeletedEvents, &QListWidget::customContextMenuRequested, this, [this](const QPoint &pos){
        QListWidgetItem *item = listDeletedEvents->itemAt(pos);
        if (item) {
            QString id = item->data(Qt::UserRole).toString();

            QMenu menu;
            QAction *restoreAction = menu.addAction(tr("恢复事件"));
            QAction *deleteAction = menu.addAction(tr("彻底删除"));

            connect(restoreAction, &QAction::triggered, this, [this, id](){
                if (API::RevokeDeletedEvent(id)) {
                    loadDeletedEvents(); // 刷新列表
                    MyMessageBox choose(nullptr,tr("成功"),tr("事件已恢复！"),QMessageBox::Information,QMessageBox::Yes);
                    choose.button(QMessageBox::Yes)->setText(tr("我知道了"));
                    choose.exec();
                } else {
                    MyMessageBox choose(nullptr,tr("错误"),tr("恢复事件失败！"),QMessageBox::Warning,QMessageBox::Yes);
                    choose.button(QMessageBox::Yes)->setText(tr("我知道了"));
                    choose.exec();
                }
            });

            connect(deleteAction, &QAction::triggered, this, [this, id](){
                MyMessageBox choose(nullptr,tr("请确认"),tr("确定要永久删除此事件吗？\n此操作不可撤销！"),QMessageBox::Question,QMessageBox::Yes | QMessageBox::No);
                choose.button(QMessageBox::Yes)->setText(tr("确认删除"));
                choose.button(QMessageBox::No)->setText(tr("取消"));
                int reply=choose.exec();
                if (reply == QMessageBox::Yes) {
                    if (API::DeleteDeletedEvent(id)) {
                        loadDeletedEvents(); // 刷新列表
                    } else {
                        MyMessageBox choose(nullptr,tr("错误"),tr("删除事件失败！"),QMessageBox::Warning,QMessageBox::Yes);
                        choose.button(QMessageBox::Yes)->setText(tr("我知道了"));
                        choose.exec();
                    }
                }
            });

            menu.exec(listDeletedEvents->viewport()->mapToGlobal(pos));
        }
    });
}

void History::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    loadFinishedEvents();
    loadDeletedEvents();
}

void History::loadFinishedEvents()
{
    listFinishedEvents->clear();

    QJsonArray finishedEvents = API::RetrieveAllFinishedEvent();
    for (const QJsonValue &value : finishedEvents) {
        QJsonObject event = value.toObject();
        createEventItem(listFinishedEvents, event);
    }

    // 添加提示信息
    if (finishedEvents.isEmpty()) {
        QListWidgetItem *emptyItem = new QListWidgetItem(tr("没有已完成的事件"));
        emptyItem->setTextAlignment(Qt::AlignCenter);
        emptyItem->setForeground(QColor("#7f8c8d"));
        listFinishedEvents->addItem(emptyItem);
    }
}

void History::loadDeletedEvents()
{
    listDeletedEvents->clear();

    QJsonArray deletedEvents = API::RetrieveAllDeletedEvent();
    for (const QJsonValue &value : deletedEvents) {
        QJsonObject event = value.toObject();
        createEventItem(listDeletedEvents, event, true);
    }

    // 添加提示信息
    if (deletedEvents.isEmpty()) {
        QListWidgetItem *emptyItem = new QListWidgetItem(tr("没有已删除的事件"));
        emptyItem->setTextAlignment(Qt::AlignCenter);
        emptyItem->setForeground(QColor("#7f8c8d"));
        listDeletedEvents->addItem(emptyItem);
    }
}

void History::createEventItem(QListWidget *listWidget, const QJsonObject &event, bool isDeleted)
{
    QString id = event["id"].toString();
    QString title = event["title"].toString();
    QString from = event["editTime"].toString();
    QString priority = QString::number(event["priority"].toInt());
    QString startStr = event["start"].toString();
    QString endStr = event["end"].toString();

    // 解析时间
    QDateTime startTime = QDateTime::fromString(startStr, Qt::ISODate);
    QDateTime endTime = QDateTime::fromString(endStr, Qt::ISODate);

    // 格式化时间
    QString timeRange = startTime.toString("yyyy-MM-dd hh:mm") + " - " + endTime.toString("hh:mm");

    // 优先级文本
    QString priorityText = "优先级: " + API::change_priority(priority.toInt());
    QColor priorityColor = API::getPriorityColor(priority.toInt());

    // 创建列表项
    QListWidgetItem *item = new QListWidgetItem();
    item->setData(Qt::UserRole, id);

    // 创建自定义小部件
    QWidget *itemWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(itemWidget);
    layout->setContentsMargins(10, 8, 10, 8);
    layout->setSpacing(5);

    // 标题和优先级
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(QString("QLabel { font: bold 14px; color: %1; }").arg(isDeleted ? "#7f8c8d" : "#2c3e50"));
    titleLabel->setMaximumWidth(300);
    titleLabel->setWordWrap(true);
    headerLayout->addWidget(titleLabel, 3);

    QLabel *priorityLabel = new QLabel(priorityText);
    priorityLabel->setStyleSheet(QString(
                                     "QLabel {"
                                     "   font: 12px;"
                                     "   color: white;"
                                     "   background-color: %1;"
                                     "   border-radius: 8px;"
                                     "   padding: 3px 8px;"
                                     "}"
                                    ).arg(priorityColor.name()));
    headerLayout->addWidget(priorityLabel, 1);

    layout->addLayout(headerLayout);

    // 时间信息
    QLabel *timeLabel = new QLabel(tr("日程时间: ")+timeRange);
    timeLabel->setStyleSheet("QLabel { font: 12px; color: #7f8c8d; }");
    layout->addWidget(timeLabel);

    // 来源信息
    QLabel *fromLabel = new QLabel(tr("操作时间: ") + from);
    fromLabel->setStyleSheet("QLabel { font: 12px; color: #7f8c8d; }");
    layout->addWidget(fromLabel);

    // 如果是已删除事件，添加提示
    if (isDeleted) {
        QLabel *deletedLabel = new QLabel(tr("(已删除)"));
        deletedLabel->setStyleSheet("QLabel { font: italic 12px; color: #e74c3c; }");
        layout->addWidget(deletedLabel);
    }
    else{
        QLabel *deletedLabel = new QLabel(tr("(已完成)"));
        deletedLabel->setStyleSheet("QLabel { font: italic 12px; color: #7CFC00; }");
        layout->addWidget(deletedLabel);
    }

    itemWidget->setLayout(layout);
    item->setSizeHint(itemWidget->sizeHint());

    listWidget->addItem(item);
    listWidget->setItemWidget(item, itemWidget);
}
