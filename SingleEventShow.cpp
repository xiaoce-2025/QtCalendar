# include "SingleEventShow.h"


SingleEventShowWidget::SingleEventShowWidget(const QJsonObject &event, QWidget *parent)
    : QWidget(parent), m_event(event) {
    setupUI();

    // 初始化事件ID
    m_eventId = event["id"].toString();

    this->setStyleSheet(R"(
        QListWidget {
            background: #F8F9FA;
            border: none;
            padding: 5px;
        }
        QListWidget::item {
            padding: 12px;
            border-radius: 4px;
            margin: 3px;
        }
        QListWidget::item:hover { background: #E9ECEF; }
        QListWidget::item:selected { background: #DEE2E6; }
        )");
}


void SingleEventShowWidget::contextMenuEvent(QContextMenuEvent *event){
    // 这里以后可能还要加判断条件
    if (true) {
        QMenu menu(this);
        if (m_selectedAttachment["type"].toString() == "file") {
            QAction *openFile = menu.addAction("打开文件");
            connect(openFile, &QAction::triggered, [this]() {
                QDesktopServices::openUrl(QUrl::fromLocalFile(
                    m_selectedAttachment["path"].toString()));
            });

            QAction *openFolder = menu.addAction("打开所在文件夹");
            connect(openFolder, &QAction::triggered, [this]() {
                QFileInfo info(m_selectedAttachment["path"].toString());
                QDesktopServices::openUrl(QUrl::fromLocalFile(
                    info.absolutePath()));
            });
        } else {
            QAction *openLink = menu.addAction("打开链接");
            connect(openLink, &QAction::triggered, [this]() {
                QDesktopServices::openUrl(
                    QUrl(m_selectedAttachment["link"].toString()));
            });
        }
        menu.exec(event->globalPos());
    }
}

void SingleEventShowWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 标题区域
    QLabel *titleLabel = new QLabel(m_event["title"].toString());
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // 时间信息
    QHBoxLayout *timeLayout = new QHBoxLayout;
    addTimeField(timeLayout, "开始时间", m_event["start"].toString("NULL"));
    addTimeField(timeLayout, "结束时间", m_event["end"].toString());
    mainLayout->addLayout(timeLayout);

    // 分类和优先级
    QHBoxLayout *metaLayout = new QHBoxLayout;
    if (m_event.contains("category")) {
        QLabel *categoryLabel = new QLabel(
            QString("类型：%1").arg(m_event["category"].toString()));
        categoryLabel->setStyleSheet(getCategoryStyle());
        metaLayout->addWidget(categoryLabel);
    }

    if (m_event.contains("priority")) {
        QLabel *priorityLabel = new QLabel(
            QString("优先级：")+API::change_priority(m_event["priority"].toInt()));
        priorityLabel->setStyleSheet("color: " + API::getPriorityColor(m_event["priority"].toInt()).name());
        metaLayout->addWidget(priorityLabel);
    }
    mainLayout->addLayout(metaLayout);

    // 提醒信息
    if (m_event.contains("reminders")) {
        QLabel *reminderTitle = new QLabel("提醒设置：");
        mainLayout->addWidget(reminderTitle);

        QJsonArray reminders = m_event["reminders"].toArray();
        foreach (const QJsonValue &r, reminders) {
            QJsonObject reminder = r.toObject();
            QString text = QString("• %1：%2").arg(
                "提醒时间",
                reminder.contains("time") ?
                    QDateTime::fromString(reminder["time"].toString(), Qt::ISODate)
                        .toString("yyyy-MM-dd hh:mm") :
                    reminder["trigger"].toString());
            mainLayout->addWidget(new QLabel(text));
        }
    }

    // 附件处理
    if (m_event.contains("attachments")) {
        QLabel *attachmentTitle = new QLabel("相关附件：");
        mainLayout->addWidget(attachmentTitle);

        QJsonArray attachments = m_event["attachments"].toArray();
        foreach (const QJsonValue &a, attachments) {
            QJsonObject attachment = a.toObject();
            QString url = attachment["type"] == "file" ?
                              QUrl::fromLocalFile(attachment["path"].toString()).toString() :
                              attachment["link"].toString();
            QLabel *linkLabel = new QLabel(
                QString("<a href=\"%1\">%2</a>").arg(url, getAttachmentText(attachment)));
            linkLabel->setOpenExternalLinks(true); // 允许点击链接直接打开
            linkLabel->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(linkLabel, &QLabel::customContextMenuRequested,
                    [attachment, linkLabel](const QPoint &pos) {
                        QMenu menu;
                        if (attachment["type"].toString() == "file") {
                            QAction *openFile = menu.addAction("打开文件");
                            QObject::connect(openFile, &QAction::triggered, [attachment]() {
                                QDesktopServices::openUrl(QUrl::fromLocalFile(
                                    attachment["path"].toString()));
                            });
                            QAction *openFolder = menu.addAction("打开所在文件夹");
                            QObject::connect(openFolder, &QAction::triggered, [attachment]() {
                                QFileInfo fileInfo(attachment["path"].toString());
                                QDesktopServices::openUrl(QUrl::fromLocalFile(
                                    fileInfo.absolutePath()));
                            });
                        } else {
                            QAction *openLink = menu.addAction("打开链接");
                            QObject::connect(openLink, &QAction::triggered, [attachment]() {
                                QDesktopServices::openUrl(QUrl(attachment["link"].toString()));
                            });
                        }
                        menu.exec(linkLabel->mapToGlobal(pos));
                    });
            mainLayout->addWidget(linkLabel);
        }
    }

    // 事件描述
    QHBoxLayout *metaLayout1 = new QHBoxLayout;
    if (m_event.contains("description")) {
        QLabel *descriptionLabel = new QLabel(
            QString("描述：%1").arg(m_event["description"].toString()));
        descriptionLabel->setWordWrap(true);
        metaLayout1->addWidget(descriptionLabel);
    }
    mainLayout->addLayout(metaLayout1);

    // 来源
    QHBoxLayout *metaLayout2 = new QHBoxLayout;
    if (m_event.contains("from")) {
        QLabel *descriptionLabel = new QLabel(
            QString("日程来源：%1").arg(m_event["from"].toString()));
        descriptionLabel->setWordWrap(true);
        metaLayout2->addWidget(descriptionLabel);
    }
    mainLayout->addLayout(metaLayout2);

    // 操作按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();

    deleteBtn = new QPushButton("删除", this);
    editBtn = new QPushButton("修改", this);
    finishBtn = new QPushButton("已完成", this);

    // 按钮样式
    deleteBtn->setStyleSheet("QPushButton { background-color: #ff4444; color: white; }");
    editBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    finishBtn->setStyleSheet("QPushButton { background-color: #007BFF; color: white; }");

    // 已完成按钮显示条件
    QDateTime eventTime;
    bool isTimeValid = false; 
    if (m_event.contains("end") && !m_event["end"].toString().isEmpty() && m_event["end"].toString() != "NULL") {
        eventTime = QDateTime::fromString(m_event["end"].toString(), Qt::ISODate);
        isTimeValid = eventTime.isValid();
    } else {
        eventTime = QDateTime::fromString(m_event["start"].toString(), Qt::ISODate);
        isTimeValid = eventTime.isValid();
    }
    finishBtn->setVisible(isTimeValid && eventTime <= QDateTime::currentDateTime());

    btnLayout->addStretch();
    btnLayout->addWidget(finishBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addWidget(editBtn);

    mainLayout->addLayout(btnLayout);

    connect(deleteBtn, &QPushButton::clicked, [this]() {
        emit deleteRequested(m_eventId);
    });
    connect(editBtn, &QPushButton::clicked, [this]() {
        emit editRequested(m_event);
    });
    connect(finishBtn, &QPushButton::clicked, [this]() {
        emit finishRequested(m_event);
    });

    mainLayout->addStretch();
}

void SingleEventShowWidget::addTimeField(QHBoxLayout *layout, const QString &label, const QString &value) {
    if (value != "NULL") {
        QLabel *timeLabel = new QLabel(
            QString("%1：%2").arg(label,
                                  QDateTime::fromString(value, Qt::ISODate)
                                      .toString("yyyy-MM-dd hh:mm")));
        layout->addWidget(timeLabel);
    }
}

QString SingleEventShowWidget::getCategoryStyle() {
    static QHash<QString, QString> styles = {
        {"ddl", "background-color: #ffcccc; padding: 2px 5px; border-radius: 3px;"},
        {"event", "background-color: #ccffcc; padding: 2px 5px; border-radius: 3px;"}
    };
    return styles.value(m_event["category"].toString(), "");
}

QString SingleEventShowWidget::getAttachmentText(const QJsonObject &attachment) {
    if (attachment["type"] == "file") {
        QFileInfo info(attachment["path"].toString());
        return QString("📎 %1").arg(info.fileName());
    }
    return QString("🌐 %1").arg(attachment["link"].toString());
}
