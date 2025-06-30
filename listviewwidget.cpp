#include "listviewwidget.h"
#include "eventwidget.h"
#include "mymessagebox.h"
#include <QSplitter>
#include <QJsonArray>
#include <QDate>

ListViewWidget::ListViewWidget(QWidget *parent) : QWidget(parent) {
    setupUI();

    connect(scheduleListWidget, &QListWidget::itemClicked,
            this, &ListViewWidget::onItemClicked);

    // 列表样式
    scheduleListWidget->setStyleSheet(R"(
        QListWidget {
            background: #f8f9fa;
            border-radius: 8px;
            padding: 5px;
            outline: none;
        }
        QListWidget::item {
            padding: 12px 8px;
            border-bottom: 1px solid #dee2e6;
            margin: 0;
            border: none;
            outline: none;
        }
        QListWidget::item:hover {
            background: #e9ecef;
            border-radius: 4px;
            outline: none;
        }
        QListWidget::item:selected {
            background: #e9ecef;
            color: inherit;
            border-radius: 4px;
            outline: none;
        }
        QListWidget::item:focus {
            outline: none;
            border: none;
        }
        QListWidget:disabled { background: #F0F0F0; }
        QListWidget::item:disabled { color: #999; }
    )");

    this->resize(1200,675);
    setRange(API::RetrieveAllEvent,QDate());
}

void ListViewWidget::setupUI() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // 水平可移动分割器
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    // 左侧
    leftWidget = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);

    eventsTitle = new QLabel("全部日程");
    eventsTitle->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #2c3e50; }");
    leftLayout->addWidget(eventsTitle);

    // 列表部件
    scheduleListWidget = new QListWidget(leftWidget);
    scheduleListWidget->setMinimumWidth(250);
    leftLayout->addWidget(scheduleListWidget);

    // 添加事件按钮
    addButton = new QPushButton("+ 添加日程", leftWidget);
    addButton->setFixedHeight(40);
    addButton->setStyleSheet(R"(
        QPushButton {
            background: #4dabf7;
            color: white;
            border: none;
            border-radius: 6px;
            font-size: 14px;
            margin: 0 20px;
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
    leftLayout->addWidget(addButton, 0, Qt::AlignBottom);
    connect(addButton, &QPushButton::clicked, this, &ListViewWidget::onAddButtonClicked);

    // 返回按钮
    backButton = new QPushButton("返回", leftWidget);
    backButton->setFixedHeight(40);
    backButton->setStyleSheet(R"(
        QPushButton {
            background: #6c757d;
            color: white;
            border: none;
            border-radius: 6px;
            font-size: 14px;
            margin: 0 20px;
        }
        QPushButton:hover {
            background: #5a6268;
        }
        QPushButton:pressed {
            background: #4a5056;
        }
        QPushButton:disabled {
            background: #cccccc;
            color: #666666;
        }
    )");
    leftLayout->addWidget(backButton, 0, Qt::AlignBottom);
    connect(backButton, &QPushButton::clicked, this, &ListViewWidget::onBackButtonClicked);

    splitter->addWidget(leftWidget);

    // 右侧详情容器
    m_detailContainer = new QWidget(this);
    m_detailContainer->setStyleSheet("background: #FFFFFF; border-left: 1px solid #E0E0E0;");
    QVBoxLayout *detailLayout = new QVBoxLayout(m_detailContainer);
    tip = new QLabel("选择事件查看详情", m_detailContainer);
    detailLayout->addWidget(tip, 0, Qt::AlignCenter);
    splitter->addWidget(m_detailContainer);

    // 设置分割器初始比例
    splitter->setSizes({250, 750});

    mainLayout->addWidget(splitter);

    setLayout(mainLayout);
}


void ListViewWidget::setSchedules(const QStringList& schedules) {
    scheduleListWidget->clear();
    for (const QString& schedule : schedules) {
        scheduleListWidget->addItem(schedule);
    }
}


void ListViewWidget::setEvents(const QJsonArray &events) {
    m_events = events;
    scheduleListWidget->blockSignals(true);
    scheduleListWidget->clear();

    if (m_events.isEmpty()) {
        // 暂无事件提示
        QListWidgetItem *noEventItem = new QListWidgetItem("暂无事件");
        noEventItem->setFlags(noEventItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
        noEventItem->setTextAlignment(Qt::AlignCenter);
        scheduleListWidget->addItem(noEventItem);
        scheduleListWidget->setEnabled(false);
    }
    else {
        for (const QJsonValue &value : m_events) {
            if (!value.isObject()) continue;
            QJsonObject event = value.toObject();

            QString datePrefix = API::formatEventDate(event);
            QString title = event.value("title").toString();

            QListWidgetItem *item = new QListWidgetItem(
                QString("%1 %2").arg(datePrefix, title)
                );

            QDateTime startTime = QDateTime::fromString(event["start"].toString(), Qt::ISODate);
            item->setData(Qt::UserRole + 1, startTime);

            item->setData(Qt::UserRole, event);
            if (event.contains("priority")) {
                int priority = event["priority"].toInt();
                item->setForeground(API::getPriorityColor(priority));
            }
            scheduleListWidget->addItem(item);
        }
        scheduleListWidget->setEnabled(true);
    }

    // 升序排序
    scheduleListWidget->sortItems(Qt::AscendingOrder);
    // 恢复信号
    scheduleListWidget->blockSignals(false);
}


// 点击事件处理
void ListViewWidget::onItemClicked(QListWidgetItem *item) {
    qDebug()<<"尝试显示事件详情";
    qDebug()<<item<<m_currentDetail;
    try {
        if (!item) {
            qDebug()<<"ERROR# 8001";
            clearDetailView();
            return;
        }
        qDebug()<<"001进程检测位置#001";

        // 清除旧详情组件
        if (m_currentDetail) {
            disconnect(m_currentDetail, nullptr, this, nullptr);
            m_currentDetail->deleteLater();
            m_detailContainer->layout()->removeWidget(m_currentDetail);
            m_currentDetail = nullptr;
        }
        qDebug()<<"001进程检测位置#002";

        // 创建新详情组件
        QJsonObject event = item->data(Qt::UserRole).toJsonObject();
        m_currentDetail = new SingleEventShowWidget(event, m_detailContainer);
        qDebug()<<"001进程检测位置#003";

        // 连接新组件信号
        connect(m_currentDetail, &SingleEventShowWidget::deleteRequested,
                this, &ListViewWidget::handleDeleteRequest);
        connect(m_currentDetail, &SingleEventShowWidget::editRequested,
                this, &ListViewWidget::handleEditRequest);
        connect(m_currentDetail, &SingleEventShowWidget::finishRequested,
                this, &ListViewWidget::handleFinishRequest);
        qDebug()<<"001进程检测位置#004";

        // 更新容器布局
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(m_detailContainer->layout());
        layout->insertWidget(0, m_currentDetail, 1);
        layout->setContentsMargins(20, 10, 20, 10);
        qDebug()<<"001进程检测位置#005";

        // 隐藏提示文字
        // tip->hide();
        qDebug()<<"001进程检测位置#006";

    } catch (const std::exception &e) {
        qDebug() << "详情加载失败:" << e.what();
        MyMessageBox::critical(this, "错误", "无法显示事件详情");
    }
    qDebug()<<"001进程检测位置#007";
    return ;
}


// 事件已完成槽函数
void ListViewWidget::handleFinishRequest(const QJsonObject &event) {
    if (API::FinishEvent(event)) {
        clearDetailView();
        refreshData();
    } else {
        MyMessageBox::critical(this, "错误", "操作失败");
    }
}


// 删除事件槽函数
void ListViewWidget::handleDeleteRequest(const QString &eventId) {
    // 二次确认
    MyMessageBox choose(nullptr,tr("请确认"),tr("确定要永久删除此日程吗？\n你将永久失去它（真的很久！）"),QMessageBox::Question,QMessageBox::Yes | QMessageBox::No);
    choose.button(QMessageBox::Yes)->setText(tr("删除"));
    choose.button(QMessageBox::No)->setText(tr("取消"));
    int reply=choose.exec();
    if (reply == QMessageBox::Yes) {
        if (API::DeleteEvent(eventId)) {
            clearDetailView();
            refreshData();
            MyMessageBox success_messagebox(nullptr,tr("成功"),tr("日程已删除"),QMessageBox::Information,QMessageBox::Yes);
            success_messagebox.button(QMessageBox::Yes)->setText(tr("返回"));
            success_messagebox.exec();
        } else {
            MyMessageBox::critical(this, "错误", "删除操作失败");
        }
    }
}


// 编辑事件槽函数
void ListViewWidget::handleEditRequest(const QJsonObject &event) {
    // 创建编辑组件
    EventWidget *editor = new EventWidget(event, m_detailContainer);
    editor->setAttribute(Qt::WA_DeleteOnClose);

    // 禁用列表交互
    setListEnabled(false);

    // 连接编辑完成信号
    connect(editor, &EventWidget::finish, this, [this, event](bool success) {
        setListEnabled(true);  // 恢复列表交互
        // 确认和取消的返回后均回到刚刚编辑的条目
        if (true) {
            // 刷新数据后显示编辑后的条目
            refreshData();

            // 查找并选中刚编辑的条目
            const QString targetId = event["id"].toString();
            for (int i = 0; i < scheduleListWidget->count(); i++) {
                QListWidgetItem* item = scheduleListWidget->item(i);
                if (item->data(Qt::UserRole).toJsonObject()["id"].toString() == targetId) {
                    scheduleListWidget->setCurrentItem(item);
                    onItemClicked(item);
                    break;
                }
            }
        }
    });

    // 连接窗口关闭信号（对应取消编辑）
    connect(editor, &EventWidget::destroyed, this, [this] {
        setListEnabled(true);
        refreshData();
    });
    // 替换右侧显示
    replaceDetailWidget(editor);
}

// 新增辅助函数
void ListViewWidget::refreshData() {
    QJsonArray events = SearchMethod();
    setEvents(events);
}

void ListViewWidget::clearDetailView() {
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(m_detailContainer->layout());
    if (!layout) return;

    // 删除详情组件
    if (layout->count() > 0) {
        QLayoutItem *item = layout->takeAt(0);
        if (item) {
            QWidget *widget = item->widget();
            if (widget) {
                widget->deleteLater();
                if (widget == m_currentDetail) {
                    m_currentDetail = nullptr;
                }
            }
            delete item;
        }
    }

    // 显示提示信息
    //tip->show();
}

void ListViewWidget::replaceDetailWidget(QWidget *newWidget) {
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(m_detailContainer->layout());

    // 清除旧组件
    for (int i=0; i<layout->count(); i++){
        if(QLabel* tip = qobject_cast<QLabel*>(layout->itemAt(i)->widget())){
            tip->hide();
        }
    }
    clearDetailView();

    // 添加新组件
    layout->insertWidget(0, newWidget);
    newWidget->show();

    // 隐藏提示信息
    // tip->hide();
}


// 编辑完成处理
void ListViewWidget::handleEditFinished(bool success) {
    if (success) {
        refreshData();
        // 此处可能要介入函数使之显示编辑完成的事件
    }
}


// 添加事件按钮
void ListViewWidget::onAddButtonClicked() {
    // 禁用列表和按钮
    setListEnabled(false);

    // 创建空白事件编辑界面
    EventWidget *editor = new EventWidget(QJsonObject(), m_detailContainer);
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->setDay(sday);

    // 连接完成信号
    connect(editor, &EventWidget::finish, this, [this](bool success) {
        setListEnabled(true);
        refreshData();
        clearDetailView();
    });

    // 连接窗口关闭信号（处理取消情况）
    connect(editor, &EventWidget::destroyed, this, [this] {
        setListEnabled(true);
    });

    replaceDetailWidget(editor);
}


// 控制左侧列表和添加事件是否可点
void ListViewWidget::setListEnabled(bool enabled) {
    scheduleListWidget->setEnabled(enabled);
    scheduleListWidget->setFocusPolicy(enabled ? Qt::StrongFocus : Qt::NoFocus);
    addButton->setEnabled(enabled);
    backButton->setEnabled(enabled);
}


// 返回按钮
void ListViewWidget::onBackButtonClicked() {
    clearAllCache();
    close();
    emit backRequested();
}


// 清除
void ListViewWidget::clearAllCache() {
    m_events = QJsonArray();
    setEvents(m_events);
    clearDetailView();
    scheduleListWidget->clearSelection();

    //tip->show();
}


// 定义查询事件函数
void ListViewWidget::setRange(std::function<QJsonArray()> f,QDate date){
    SearchMethod = f;
    setEvents(f());
    if (date.isValid()){
        sday=date;
        eventsTitle->setText(date.toString("yyyy-MM-dd日程"));
    }
    else{
        sday = QDate::currentDate();
        eventsTitle->setText("全部事件");
    }
    return ;
}


ListViewWidget::~ListViewWidget() {
    qDebug()<<"ListViewWidget已析构";
    if (m_currentDetail) {
        disconnect(m_currentDetail, nullptr, this, nullptr);
        m_currentDetail->deleteLater();
    }
    scheduleListWidget->clear();
}


void ListViewWidget::mustnotback(){
    backButton->setCheckable(false);
    backButton->hide();
}
