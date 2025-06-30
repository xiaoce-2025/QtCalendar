#include "homepage.h"
#include "api.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QFont>
#include "basicvariable.h"

// 这个是北京海淀的特征码，后续如果可以的话可以拓展成自动识别定位（？
const QString area = "101010200";

HomePage::HomePage(QWidget *parent) : QWidget(parent) {
    // 创建网络访问管理器
    netManager = new QNetworkAccessManager(this);
    connect(netManager, &QNetworkAccessManager::finished, this, &HomePage::handleWeatherResponse);
    mainLayout = new QHBoxLayout(this);
    this->resize(1000,675);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(15);

    fetchWeatherData(area);

    createInfoPanel();

    updateDateTime();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &HomePage::updateDateTime);
    timer->start(1000);
}


// 发起天气请求
void HomePage::fetchWeatherData(const QString &cityCode) {
    qDebug()<<"已向服务器提交天气请求。地区id:"<<cityCode;
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityCode);
    netManager->get(QNetworkRequest(url));
}

// 处理天气API响应
void HomePage::handleWeatherResponse(QNetworkReply *reply) {
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Weather API error:" << reply->errorString();
        updateWeatherUI("未知", "未知", "N/A", "");
        reply->deleteLater();
        return;
    }

    // 检查HTTP状态码
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status != 200) {
        qDebug() << "Weather API HTTP error:" << status;
        updateWeatherUI("未知", "未知", "N/A", "");
        reply->deleteLater();
        return;
    }

    // 解析JSON响应
    QByteArray data = reply->readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << error.errorString();
        updateWeatherUI("未知", "未知", "N/A", "");
        reply->deleteLater();
        return;
    }

    QJsonObject rootObj = doc.object();
    //qDebug()<<"天气数据请求成功: "<<rootObj;
    qDebug()<<"天气数据请求成功";
    // 检查API返回状态
    if (rootObj["status"].toInt() != 200) {
        qDebug() << "Weather API error:" << rootObj["message"].toString();
        updateWeatherUI("未知", "未知", "N/A", "");
        reply->deleteLater();
        return;
    }

    // 提取天气数据
    QString city = rootObj["cityInfo"].toObject()["parent"].toString();
    city += " ";
    city += rootObj["cityInfo"].toObject()["city"].toString();

    QJsonObject dataObj = rootObj["data"].toObject();
    QString weatherType = "未知";
    QString temperature = "N/A";
    QString description = "";

    // 检查forecast数组
    if (dataObj.contains("forecast") && dataObj["forecast"].isArray()) {
        QJsonArray forecastArray = dataObj["forecast"].toArray();
        if (!forecastArray.isEmpty()) {
            QJsonObject todayWeather = forecastArray.first().toObject();
            weatherType = todayWeather["type"].toString();

            // 提取温度范围
            QString high = todayWeather["high"].toString().split(" ").last().replace("℃", "");
            QString low = todayWeather["low"].toString().split(" ").last().replace("℃", "");
            temperature = low + "~" + high + "°C";

            // 构建描述字符串
            description = weatherType;
            if (todayWeather.contains("fx") && todayWeather.contains("fl")) {
                description += "，" + todayWeather["fx"].toString() + todayWeather["fl"].toString();
            }
        }
    }

    // 更新UI
    updateWeatherUI(city, weatherType, temperature, description);

    reply->deleteLater();
}

// 更新天气UI组件
void HomePage::updateWeatherUI(const QString &city,
                               const QString &type,
                               const QString &temp,
                               const QString &desc) {
    weatherCity->setText(city);
    weatherTemp->setText(temp);

    // 设置天气描述，如果为空则使用天气类型
    if (desc.isEmpty()) {
        weatherDescription->setText(type);
    } else {
        weatherDescription->setText(desc);
    }

    // 根据天气类型设置图标
    QString iconPath = determineWeatherIcon(type);
    // 加载图片并缩放以适应标签
    QPixmap pixmap(iconPath);
    if (!pixmap.isNull()) {
        // 缩放图片并保持宽高比
        pixmap = pixmap.scaled(weatherIcon->width(),weatherIcon->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        // 设置到标签
        weatherIcon->setPixmap(pixmap);
    } else {
        // 加载失败时清除图片
        weatherIcon->clear();
        qWarning() << "无法加载天气图标:" << iconPath;
    }
}

// 根据天气类型确定图标路径
QString HomePage::determineWeatherIcon(const QString &type) {
    if (type.contains("晴")) return ":/weather_icon/sunny.png";
    if (type.contains("雷阵雨")) return ":/weather_icon/tstorms.png";
    if (type.contains("雾")) return ":/weather_icon/fog.png  ";
    if (type.contains("雪")) return ":/weather_icon/snow.png";
    if (type.contains("多云")) return ":/weather_icon/mostlycloudy.png";
    if (type.contains("阴")) return ":/weather_icon/cloudy.png";
    if (type.contains("雨")) return ":/weather_icon/rain.png";
    return ":/weather_icon/unknown.png"; // 默认图标
}


void HomePage::updateDateTime() {
    QDateTime now = QDateTime::currentDateTime();
    labelTime->setText(now.toString("HH:mm:ss"));
    labelDate->setText(now.toString("yyyy年MM月dd日"));
    labelDay->setText(now.toString("dddd"));
}

void HomePage::createInfoPanel() {
    // 主面板容器
    QWidget *mainPanel = new QWidget(this);
    mainPanel->setObjectName("mainPanel");
    mainPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainPanel->setStyleSheet(R"(
        QWidget#mainPanel {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4189c7, stop:1 #4ac6f4);
            border-radius: 0px;
            padding: 25px;
        }
    )");

    // 主水平布局 - 左右分区
    QHBoxLayout *mainPanelLayout = new QHBoxLayout(mainPanel);
    mainPanelLayout->setSpacing(25);
    mainPanelLayout->setContentsMargins(0, 0, 0, 0);

    // ========= 左侧区域 =========
    QWidget *leftPanel = new QWidget(mainPanel);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(20);
    leftLayout->setContentsMargins(20, 20, 5, 20);

    // 顶部：日期时间面板
    QWidget *dateTimePanel = new QWidget(leftPanel);
    dateTimePanel->setObjectName("dateTimePanel");
    dateTimePanel->setStyleSheet(R"(
        QWidget#dateTimePanel {
            background: rgba(255, 255, 255, 0.18);
            border-radius: 12px;
            padding: 12px 18px;
        }
    )");

    QVBoxLayout *dateLayout = new QVBoxLayout(dateTimePanel);
    dateLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    dateLayout->setSpacing(10);

    // 星期几显示
    labelDay = new QLabel("星期三");
    labelDay->setStyleSheet(R"(
        QLabel {
            color: white;
            font-size: 36px;
            font-weight: bold;
        }
    )");

    // 日期显示
    labelDate = new QLabel("2023年12月12日");
    labelDate->setStyleSheet(R"(
        QLabel {
            color: white;
            font-size: 30px;
            font-weight: bold;
        }
    )");

    // 时间区域
    QWidget *timeWidget = new QWidget;
    QHBoxLayout *timeLayout = new QHBoxLayout(timeWidget);
    timeLayout->setContentsMargins(0, 5, 0, 0);
    timeLayout->setSpacing(8);

    QLabel *timeLabel = new QLabel("当前时间:");
    timeLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");

    labelTime = new QLabel;
    labelTime->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");

    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(labelTime);

    dateLayout->addWidget(labelDay);
    dateLayout->addWidget(labelDate);
    dateLayout->addWidget(timeWidget);
    dateLayout->addStretch();

    // 底部：天气面板
    QGroupBox *weatherBox = new QGroupBox("今日天气");
    weatherBox->setObjectName("weatherBox");
    weatherBox->setStyleSheet(R"(
        QGroupBox#weatherBox {
            color: white;
            font-size: 22px;
            font-weight: bold;
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 12px;
            padding: 35px 20px 20px 20px;  /* 顶部增加内边距 */
            background: rgba(255, 255, 255, 0.15);
        }
        QGroupBox::title {
            subcontrol-origin: padding;
            subcontrol-position: top center;
            padding: 0 15px;
            top: 15px;
            border-radius: 8px;
        }
    )");

    QVBoxLayout *weatherLayout = new QVBoxLayout(weatherBox);
    weatherLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    weatherLayout->setSpacing(12);

    weatherIcon = new QLabel;
    weatherIcon->setFixedSize(110, 110);
    //weatherIcon->setStyleSheet("background: url(:/weather_icon/unknown.png);");
    weatherIcon->setAlignment(Qt::AlignCenter);

    weatherTemp = new QLabel("暂无温度信息");
    weatherTemp->setStyleSheet("color: white; font-size: 36px; font-weight: bold;");

    weatherCity = new QLabel("北京市");
    weatherCity->setStyleSheet("color: white; font-size: 22px; font-weight: bold;");

    weatherDescription = new QLabel("暂无天气信息，请检查网络连接");
    weatherDescription->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");

    weatherLayout->addWidget(weatherIcon, 0, Qt::AlignHCenter);
    weatherLayout->addWidget(weatherTemp, 0, Qt::AlignHCenter);
    weatherLayout->addWidget(weatherCity, 0, Qt::AlignHCenter);
    weatherLayout->addWidget(weatherDescription, 0, Qt::AlignHCenter);
    weatherLayout->addStretch();

    // 左侧布局组合
    leftLayout->addWidget(dateTimePanel);
    leftLayout->addWidget(weatherBox);
    leftLayout->setStretch(0, 45); // 日期时间部分自适应
    leftLayout->setStretch(1, 55); // 天气部分自适应

    // ========= 右侧区域 =========
    QWidget *rightPanel = new QWidget(mainPanel);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setSpacing(25);
    rightLayout->setContentsMargins(5, 20, 20, 20);

    // 提醒区域
    QGroupBox *remindersBox = new QGroupBox("今日提醒");
    remindersBox->setObjectName("remindersBox");
    remindersBox->setStyleSheet(R"(
        QGroupBox#remindersBox {
            color: white;
            font-size: 22px;
            font-weight: bold;
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 12px;
            padding: 45px 20px 20px 20px;  /* 顶部增加内边距 */
            background: rgba(255, 255, 255, 0.15);
        }
        QGroupBox::title {
            subcontrol-origin: padding;
            subcontrol-position: top center;
            padding: 0 15px;
            top: 12px;
            border-radius: 8px;
        }
    )");

    QVBoxLayout *remindersLayout = new QVBoxLayout(remindersBox);
    remindersLayout->setSpacing(10);
    remindersLayout->setContentsMargins(5, 5, 5, 5);

    // 获取档期所有事件
    auto f = API::RetrieveFromDay(QDate::currentDate());
    QJsonArray events = f();
    QStringList reminders;
    // 排序
    QList<QPair<QDateTime, QString>> eventList;
    for (const QJsonValue &value : events) {
        if (!value.isObject()) continue;
        QJsonObject event = value.toObject();
        QDateTime start = QDateTime::fromString(event["start"].toString(), Qt::ISODate);
        QDateTime end = QDateTime::fromString(event["end"].toString(), Qt::ISODate);

        // 构建显示字符串
        QString sss = QString(start.isValid() ? start.time().toString("hh:mm") : "        ")
                      + " - " + QString(end.isValid() ? end.time().toString("hh:mm") : "        ")
                      + " " + event["title"].toString();

        // 确定排序依据的时间
        QDateTime sortTime;
        if (start.isValid()) {
            sortTime = start;
        } else if (end.isValid()) {
            sortTime = end;
        } else {
            // 两个时间都无效，设置为最大日期时间（排在最后）
            sortTime = QDateTime(QDate(9999, 12, 31), QTime(23, 59, 59));
        }

        eventList.append(qMakePair(sortTime, sss));
    }
    // 使用 Lambda 表达式进行排序
    std::sort(eventList.begin(), eventList.end(), [](const QPair<QDateTime, QString> &a, const QPair<QDateTime, QString> &b) {
        return a.first < b.first;
    });
    // 将排序结果转到QStringList中
    reminders.reserve(eventList.size());
    for (const auto &item : eventList) {
        reminders.append(item.second);
    }

    // 若无日程则增加提示信息
    if (reminders.isEmpty()){
        reminders.append("事情都办完了呢！\n今天真是摸鱼的好日子呀(≧▽≦)o.｡");
    }

    // 显示事件在首页（最多6项）
    int temp_1 = 0;
    for (const QString &reminder : reminders) {
        temp_1++;
        if (temp_1 > 6){
            break;
        }
        QLabel *reminderLabel = new QLabel(reminder);
        reminderLabel->setStyleSheet(R"(
            QLabel {
                color: white;
                font-size: 18px;
                padding: 12px 16px;
                border-bottom: 1px solid rgba(255, 255, 255, 0.2);
                background: rgba(255, 255, 255, 0.1);
                border-radius: 8px;
            }
        )");
        reminderLabel->setMinimumHeight(50);
        remindersLayout->addWidget(reminderLabel);
    }

    // 按钮区域
    QWidget *buttonWidget = new QWidget;
    QGridLayout *buttonLayout = new QGridLayout(buttonWidget);
    buttonLayout->setSpacing(18);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    const QString buttonStyle = R"(
        QPushButton {
            background: rgba(255, 255, 255, 0.25);
            color: white;
            border-radius: 10px;
            padding: 16px 25px;
            font-size: 18px;
            font-weight: bold;
            min-width: 120px;
            outline: none;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.35);
        }
        QPushButton:pressed {
            background: rgba(255, 255, 255, 0.15);
        }
    )";

    QPushButton *btnWeather = new QPushButton("天气预报");
    btnWeather->setStyleSheet(buttonStyle);
    connect(btnWeather, &QPushButton::clicked, [this]() {
        QDesktopServices::openUrl(QUrl("https://www.weather.com.cn/weather/"+area+".shtml"));
    });

    QPushButton *btnView = new QPushButton("使用文档");
    btnView->setStyleSheet(buttonStyle);
    connect(btnView, &QPushButton::clicked, [this]() {
        QDesktopServices::openUrl(QUrl("https://github.com/xiaoce-2025/QtCalendar/blob/main/%E7%94%A8%E6%88%B7%E6%89%8B%E5%86%8C.md"));
    });

    QPushButton *btnSchedule = new QPushButton("关于我们");
    btnSchedule->setStyleSheet(buttonStyle);
    connect(btnSchedule, &QPushButton::clicked, [this]() {
        QDesktopServices::openUrl(QUrl("https://github.com/xiaoce-2025/QtCalendar/blob/main/README.md"));
    });

    QPushButton *btnExit = new QPushButton("退出");
    btnExit->setStyleSheet(buttonStyle);
    connect(btnExit, &QPushButton::clicked, qApp, &QApplication::quit);

    // 使用网格布局使按钮均匀排列
    buttonLayout->addWidget(btnWeather, 0, 0);
    buttonLayout->addWidget(btnView, 0, 1);
    buttonLayout->addWidget(btnSchedule, 1, 0);
    buttonLayout->addWidget(btnExit, 1, 1);
    buttonLayout->setRowStretch(0, 1);
    buttonLayout->setRowStretch(1, 1);
    buttonLayout->setColumnStretch(0, 1);
    buttonLayout->setColumnStretch(1, 1);

    // 右侧布局组合
    rightLayout->addWidget(remindersBox);
    rightLayout->addWidget(buttonWidget);
    rightLayout->setStretch(0, 70); // 提醒部分占2/3高度
    rightLayout->setStretch(1, 30); // 按钮部分占1/3高度

    // ========= 组合主面板 =========
    mainPanelLayout->addWidget(leftPanel, 45); // 左侧占比40%
    mainPanelLayout->addWidget(rightPanel, 55); // 右侧占比60%

    // 确保主面板填满主布局空间
    mainLayout->addWidget(mainPanel);
    this->setLayout(mainLayout); // 将主布局设置到窗口
}
