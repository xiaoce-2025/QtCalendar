#include "calendarwidget.h"
#include "SourceAPI.h"
#include "listviewwidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QLabel>
#include <QEvent>
#include <QScrollBar>
#include <QDateTime>
#include <QTimer>
#include <QString>
#include <QPushButton>

CalendarWidget::CalendarWidget(QWidget *parent) : QWidget(parent) {
    resize(1200,675);
    setupUI();
    setWindowTitle("日程日历");

    calendar->setSelectedDate(QDate::currentDate());
    onDateChanged();

    connect(calendar, &QCalendarWidget::selectionChanged, this, &CalendarWidget::onDateChanged);
    scheduleList->viewport()->installEventFilter(this);

    // 强制刷新
    QTimer::singleShot(0, [this]{
        onDateChanged();
        calendar->refreshCells();
    });
}

void CalendarWidget::setupUI() {
    stackedWidget = new QStackedWidget(this);

    // 主日历界面
    mainCalendarWidget = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(mainCalendarWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // 左侧日历
    QWidget *leftPanel = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    calendar = new CustomCalendarWidget;
    calendar->setMinimumSize(600, 400);
    leftLayout->addWidget(calendar);
    mainLayout->addWidget(leftPanel, 2);

    calendar->setScheduleMap(&scheduleMapTitle);

    // 右侧事件
    rightPanel = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    eventsTitle = new QLabel("当日事件");
    eventsTitle->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #2c3e50; }");
    rightLayout->addWidget(eventsTitle);

    scheduleList = new QListWidget;
    scheduleList->setStyleSheet(R"(
            QListWidget {
                background: #f8f9fa;
                border-radius: 8px;
                padding: 5px;
            }
            QListWidget::item {
                border-bottom: 1px solid #dee2e6;
            }
            QListWidget::item:hover {
                background: #e9ecef;
            }
        )");
    scheduleList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用水平滚动条
    rightLayout->addWidget(scheduleList);

    // 查看详情按钮
    detailButton = new QPushButton("查看详情");
    detailButton->setStyleSheet(R"(
        QPushButton {
            background: #4dabf7;
            color: white;
            border: none;
            border-radius: 6px;
            font-size: 14px;
            margin: 0 20px;
            min-height: 40px;
        }
        QPushButton:hover {
            background: #339af0;
        }
        QPushButton:pressed {
            background: #228be6;
        }
        QPushButton:disabled {
            background: #cccccc;
            color: #666666;
        }
    )");
    rightLayout->addWidget(detailButton);
    connect(detailButton, &QPushButton::clicked, this, &CalendarWidget::onDetailButtonClicked);

    // 返回按钮
    backButton = new QPushButton("返回");
    backButton->setStyleSheet(R"(
        QPushButton {
            background: #6c757d;  /* 改为中性灰色 */
            color: white;
            border: none;
            border-radius: 6px;
            font-size: 14px;
            margin: 0 20px;
            min-height: 40px;
        }
        QPushButton:hover {
            background: #5a6268;  /* 深灰色悬停 */
        }
        QPushButton:pressed {
            background: #4a5056;  /* 更深灰色按下 */
        }
        QPushButton:disabled {
            background: #cccccc;
            color: #666666;
        }
    )");
    rightLayout->addWidget(backButton);
    connect(backButton, &QPushButton::clicked, this, &CalendarWidget::onBackButtonClicked);

    mainLayout->addWidget(rightPanel, 1);

    setStyleSheet(R"(
            QCalendarWidget {
                background: white;
                border-radius: 8px;
            }
            QCalendarWidget QToolButton {
                font-size: 14px;
                color: #2c3e50;
            }
            QCalendarWidget QMenu {
                background: white;
            }
            QCalendarWidget QSpinBox {
                width: 80px;
                font-size: 14px;
            }
        )");

    // 列表视图（查看详情的象形）
    listViewWidget = new ListViewWidget;
    connect(listViewWidget, &ListViewWidget::backRequested, this, [this]() {
        stackedWidget->setCurrentWidget(mainCalendarWidget);
        // 强制刷新（列表返回时不会主动刷新）
        onDateChanged();
    });

    stackedWidget->addWidget(mainCalendarWidget);
    stackedWidget->addWidget(listViewWidget);

    // 主布局
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->addWidget(stackedWidget);
}

void CalendarWidget::onDateChanged() {
    QDate selectedDate = calendar->selectedDate();
    QString currentMonth = selectedDate.toString("yyyy-MM");
    detailButton->setProperty("date", selectedDate);
    eventsTitle->setText(selectedDate.toString("yyyy年MM月dd日日程"));

    // 延迟加载
    if (!loadedMonths.contains(currentMonth)) {
        QJsonArray events = SourceAPI::RetrieveMonthEvent(currentMonth);

        for (const QJsonValue& eventValue : events) {
            QJsonObject event = eventValue.toObject();
            QString eventID = event["id"].toString();


            QDate date;
            if (event.contains("start") && !event["start"].isNull() && event["start"].toString()!="NULL") {
                QDateTime startDt = QDateTime::fromString(event["start"].toString(), Qt::ISODate);
                date = startDt.date();
            }
            else if (event.contains("end") && !event["end"].isNull() && event["end"].toString()!="NULL") {
                QDateTime endDt = QDateTime::fromString(event["end"].toString(), Qt::ISODate);
                date = endDt.date();
            }
            else{
                qDebug() << "Invalid date for event:" << eventID;
            }

            // 注册事件
            eventRegistry.insert(eventID, event);
            scheduleMap[date].append(eventID);

            QString title = event["title"].toString();
            scheduleMapTitle[date].append(title);
        }
        loadedMonths.insert(currentMonth);
    }


    qDebug() << "尝试加载月份事件数据：" << currentMonth;
    // 更新日程列表
    scheduleList->clear();
    QList<QString> eventIDs = scheduleMap.value(selectedDate);

    if (eventIDs.isEmpty()) {
        // 无事件提示
        QListWidgetItem *noEventItem = new QListWidgetItem();
        noEventItem->setFlags(Qt::NoItemFlags); //列表不可选
        QWidget *widget = new QWidget();
        QLabel *label = new QLabel("当日暂无事件\n点击“查看详情”以添加事件");
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: #666666; font-size: 12pt;");
        QHBoxLayout *layout = new QHBoxLayout(widget);
        layout->addWidget(label);
        widget->setLayout(layout);
        scheduleList->addItem(noEventItem);
        scheduleList->setItemWidget(noEventItem, widget);
    }
    else {
        foreach (const QString& eventID, scheduleMap.value(selectedDate)) {
            if (!eventRegistry.contains(eventID)) continue;

            QJsonObject event = eventRegistry[eventID];
            QDateTime start = QDateTime::fromString(event["start"].toString(), Qt::ISODate);
            QDateTime end = QDateTime::fromString(event["end"].toString(), Qt::ISODate);

            // 构建列表项
            QWidget *itemWidget = new QWidget;
            itemWidget->setMinimumWidth(250);

            QHBoxLayout *layout = new QHBoxLayout(itemWidget);
            layout->setContentsMargins(12, 8, 12, 8);
            layout->setSpacing(15);

            // 时间标签
            QLabel *timeLabel = new QLabel(
                QString("<span style='color: #495057; font-size: 10pt'>%1-%2</span>")
                    .arg(start.isValid()? start.time().toString("hh:mm") : "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")
                    .arg(end.isValid()? end.time().toString("hh:mm") : "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"));
            timeLabel->setFixedWidth(80);


            // 标题自动换行
            QLabel *titleLabel = new QLabel(event["title"].toString());
            titleLabel->setWordWrap(true);
            titleLabel->setStyleSheet("color: #2b2d42; font-size: 11pt; margin-right: 10px;");
            titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            layout->addWidget(timeLabel);
            layout->addWidget(titleLabel, 1);


            // 列表项
            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(itemWidget->sizeHint());
            scheduleList->addItem(item);
            scheduleList->setItemWidget(item, itemWidget);

            // 计算高度
            int itemWidth = scheduleList->viewport()->width();
            if (itemWidth <= 0) {
                itemWidth = rightPanel->width() - scheduleList->verticalScrollBar()->width() - 20;
            }
            // 设置默认高度（某些情况下高度无法正常计算，貌似是其他界面的一些加载顺序问题？）
            itemWidget->setFixedWidth(itemWidth);
            itemWidget->adjustSize();

            // 计算标题标签高度
            int titleHeight = titleLabel->heightForWidth(titleLabel->width());
            int totalHeight = titleHeight + layout->contentsMargins().top() + layout->contentsMargins().bottom();
            totalHeight = qMax(totalHeight, 50);

            item->setSizeHint(QSize(itemWidth, totalHeight));
        }
    }
    qDebug()<<"数据处理完成#101";

    QTimer::singleShot(0, this, &CalendarWidget::updateItemHeights);
    //日历强制刷新
    calendar->refreshCells();
}

bool CalendarWidget::eventFilter(QObject *watched, QEvent *event) {
    if (watched == scheduleList->viewport() && event->type() == QEvent::Resize) {
        updateItemHeights();
    }
    return QWidget::eventFilter(watched, event);
}

void CalendarWidget::updateItemHeights() {
    for (int i = 0; i < scheduleList->count(); ++i) {
        QListWidgetItem *item = scheduleList->item(i);
        QWidget *widget = scheduleList->itemWidget(item);
        if (widget) {
            int itemWidth = scheduleList->viewport()->width();
            widget->setFixedWidth(itemWidth);
            widget->adjustSize();

            // 检查是否为提示项
            QLabel *label = widget->findChild<QLabel*>();
            if (label && label->text() == "当日暂无事件\n点击“查看详情”以添加事件") {
                int height = label->sizeHint().height() + 20;
                item->setSizeHint(QSize(itemWidth, height));
                continue;
            }

            // 正常事件项的高度计算
            QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(widget->layout());
            if (layout) {
                QLabel *titleLabel = qobject_cast<QLabel*>(layout->itemAt(1)->widget());
                if (titleLabel) {
                    int titleHeight = titleLabel->heightForWidth(titleLabel->width());
                    int totalHeight = titleHeight + layout->contentsMargins().top() + layout->contentsMargins().bottom();
                    totalHeight = qMax(totalHeight, 50);
                    item->setSizeHint(QSize(itemWidth, totalHeight));
                }
            }
        }
    }
}


// 查看详情槽函数处理按钮点击
void CalendarWidget::onDetailButtonClicked() {
    QDate selectedDate = calendar->selectedDate();
    /*
    QJsonArray TodayList;

    foreach (const QString& eventID, scheduleMap.value(selectedDate)) {
        if (eventRegistry.contains(eventID)){
            TodayList.append(eventRegistry[eventID]);
        }
    }
    */

    // 更新列表视图数据并切换界面
    clearCache();

    // 每次采用新的listviewWidget
    stackedWidget->removeWidget(listViewWidget);
    listViewWidget->deleteLater();
    listViewWidget = new ListViewWidget;
    stackedWidget->addWidget(listViewWidget);
    connect(listViewWidget, &ListViewWidget::backRequested, this, [this]() {
        stackedWidget->setCurrentWidget(mainCalendarWidget);
        onDateChanged();
    });
    listViewWidget->setRange(API::RetrieveFromDay(selectedDate),selectedDate);
    stackedWidget->setCurrentWidget(listViewWidget);
}


// 返回槽函数处理按钮点击
void CalendarWidget::onBackButtonClicked() {
    close();
    clearCache();
    emit BackRequest();
}


void CalendarWidget::setDay(const QDate date){
    calendar->setSelectedDate(date);
}


void CalendarWidget::clearCache() {
    loadedMonths.clear();
    scheduleMap.clear();
    scheduleMapTitle.clear();
    eventRegistry.clear();
    scheduleList->clear();
}


void CalendarWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    updateItemHeights();
}


void CalendarWidget::MainWindow_Refresh(){
    onDateChanged();
}

// 进制返回
void CalendarWidget::mustnotback(){
    backButton->setCheckable(false);
    backButton->hide();
}
