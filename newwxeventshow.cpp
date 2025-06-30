#include "newwxeventshow.h"
#include "ui_newwxeventshow.h"
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QLabel>
#include <QJsonObject>
#include <QDateTime>
#include <QPushButton>
#include <QCheckBox>
#include "sourceapi.h"
#include "mymessagebox.h"
#include <QHBoxLayout>
#include <QDebug>

NewWXEventShow::NewWXEventShow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NewWXEventShow)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // 添加标题
    QLabel *titleLabel = new QLabel("小希在您不在的时间里，在微信中监听到下列日程~");
    titleLabel->setStyleSheet(
        "QLabel {"
        "   font: bold 20px '微软雅黑';"
        "   color: #2c3e50;"
        "   padding: 10px;"
        "   border-bottom: 2px solid #3498db;"
        "   background: #f8f9fa;"
        "   border-radius: 6px;"
        "}"
        );
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // 添加选择按钮区域
    QHBoxLayout *selectionLayout = new QHBoxLayout();
    selectAllButton = new QPushButton("全选");
    deselectAllButton = new QPushButton("全不选");

    // 设置按钮样式
    QString buttonStyle =
        "QPushButton {"
        "   background: #3498db;"
        "   color: white;"
        "   padding: 6px 12px;"
        "   border-radius: 4px;"
        "   font: 14px '微软雅黑';"
        "   min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "   background: #2980b9;"
        "}";

    selectAllButton->setStyleSheet(buttonStyle);
    deselectAllButton->setStyleSheet(buttonStyle);

    selectionLayout->addWidget(selectAllButton);
    selectionLayout->addWidget(deselectAllButton);
    selectionLayout->addStretch();

    mainLayout->addLayout(selectionLayout);

    // 消息列表
    listWidget = new QListWidget();
    listWidget->setStyleSheet(
        "QListWidget {"
        "   background: #f5f5f5;"
        "   border: 1px solid #ddd;"
        "   border-radius: 8px;"
        "}"
        "QListWidget::item {"
        "   border-bottom: 1px solid #eee;"
        "}"
        "QListWidget::item:hover {"
        "   background: #e3f2fd;"
        "}"
        );

    // 返回按钮
    backButton = new QPushButton("添加选中日程");
    backButton->setStyleSheet(
        "QPushButton {"
        "   background: #4CAF50;"
        "   color: white;"
        "   padding: 10px 20px;"
        "   border-radius: 6px;"
        "   font: bold 16px '微软雅黑';"
        "}"
        "QPushButton:hover {"
        "   background: #45a049;"
        "}"
        );

    mainLayout->addWidget(listWidget, 1); // 设置列表可伸缩
    mainLayout->addWidget(backButton);

    // 连接信号槽
    connect(backButton, &QPushButton::clicked, this, &NewWXEventShow::processSelectedMessages);
    connect(selectAllButton, &QPushButton::clicked, this, &NewWXEventShow::selectAll);
    connect(deselectAllButton, &QPushButton::clicked, this, &NewWXEventShow::deselectAll);
}

void NewWXEventShow::addMessage(const QJsonObject &msg) {
    // 解析消息内容
    QString dateStr = QString("%1年%2月%3日")
                          .arg(msg.value("year").toString())
                          .arg(msg.value("month").toString())
                          .arg(msg.value("day").toString());

    QString content = QString("[WeChatMsg: %1][%2] %3：%4")
.arg(msg.value("fromwho").toString())
.arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                          .arg(dateStr)
                          .arg(msg.value("event").toString()
                          );

    // 创建自定义的Widget项
    QWidget *itemWidget = new QWidget();
    QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
    itemLayout->setContentsMargins(12, 8, 12, 8);

    // 添加复选框
    QCheckBox *checkBox = new QCheckBox();
    checkBox->setChecked(true);
    checkBoxes.append(checkBox); // 保存复选框引用
    itemLayout->addWidget(checkBox);

    // 添加消息图标和内容
    QLabel *icon = new QLabel("💬");
    QLabel *text = new QLabel(content);
    text->setWordWrap(true);
    text->setStyleSheet("QLabel { font: 14px '微软雅黑'; }");

    itemLayout->addWidget(icon);
    itemLayout->addWidget(text, 1);

    // 创建列表项
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(itemWidget->sizeHint()); // 设置合适的大小
    listWidget->addItem(item);
    listWidget->setItemWidget(item, itemWidget);

    // 自动滚动到底部
    listWidget->scrollToBottom();


    QJsonObject obj;
    // 必填字段
    obj["id"] = QDateTime::currentDateTime().toString("yyMMddHHmmsszzz");
    obj["title"] = msg.value("event").toString();

    // 提取年月日并格式化为两位数
    QString year = msg.value("year").toString();
    QString month = msg.value("month").toString();
    QString day = msg.value("day").toString();
    // 确保月份是两位数（不足补0）
    if (month.length() == 1) month = "0" + month;
    // 确保日期是两位数（不足补0）
    if (day.length() == 1) day = "0" + day;
    bool temp_001 = false;
    if (msg.value("start_time").toString() != "NULL") {
        temp_001 = true;
        obj["start"] = year + "-" + month + "-" + day + "T" + msg.value("start_time").toString();
    }
    if (msg.value("end_time").toString() != "NULL") {
        temp_001 = true;
        obj["end"] = year + "-" + month + "-" + day + "T" + msg.value("end_time").toString();
    }
    if (!temp_001) {
        obj["start"] = year + "-" + month + "-" + day + "T" + "00:01";
    }

    // 描述
    obj["description"] = msg.value("description").toString();

    // 附件处理
    QJsonArray attachments;
    if (msg.value("url").toString()!="NULL") {
        QJsonObject att;
        att["type"] = "url";
        att["link"] = msg.value("url").toString();
        attachments.append(att);
    }

    if (msg.value("docu").toString()!="NULL") {
        QJsonObject att;
        att["type"] = "file";
        att["path"] = msg.value("docu").toString();
        attachments.append(att);
    }

    if (!attachments.isEmpty()) {
        obj["attachments"] = attachments;
    }

    obj["priority"] = msg.value("importance").toInt();

    obj["from"] = "WeChatMsg:" + msg.value("fromwho").toString();

    allevent.append(obj);
}

void NewWXEventShow::processSelectedMessages() {
    qDebug() << "处理选中的消息:";

    for (int i = 0; i < checkBoxes.size(); ++i) {
        if (checkBoxes[i]->isChecked()) {
            QListWidgetItem *item = listWidget->item(i);
            QWidget *widget = listWidget->itemWidget(item);

            if (widget) {
                qDebug()<<"开始处理项:"<<i;
                qDebug()<<"开始创建日程";
                if (!allevent[i].isObject()){
                    qDebug()<<"无效的日程数据格式，项"<<i;
                    MyMessageBox::critical(this, "错误", "创建事件失败");
                }
                else if (!SourceAPI::CreateEvent(allevent[i].toObject())){
                    MyMessageBox::critical(this, "错误", "创建事件失败");
                }
                else{
                    qDebug()<<"写入事件详情："<<allevent[i].toObject();
                    qDebug()<<"日程创建完毕";
                }
            }
        }
    }

    // 可以添加清除选择或关闭窗口的逻辑
    // deselectAll(); // 取消所有选择
    emit backRequested();
}

void NewWXEventShow::selectAll() {
    for (QCheckBox *checkBox : checkBoxes) {
        checkBox->setChecked(true);
    }
}

void NewWXEventShow::deselectAll() {
    for (QCheckBox *checkBox : checkBoxes) {
        checkBox->setChecked(false);
    }
}

NewWXEventShow::~NewWXEventShow()
{
    delete ui;
}
