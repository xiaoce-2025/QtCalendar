# 智能日历管理系统 - 技术API文档

## 📋 目录

1. [概述](#概述)
2. [核心类架构](#核心类架构)
3. [API接口文档](#api接口文档)
4. [数据结构](#数据结构)
5. [事件系统](#事件系统)
6. [Python集成](#python集成)
7. [数据库设计](#数据库设计)
8. [错误处理](#错误处理)
9. [性能优化](#性能优化)

## 📖 概述

本文档描述了智能日历管理系统的技术架构和API接口。系统采用Qt C++框架开发，集成了Python脚本用于微信监听和AI解析功能。

### 技术栈
- **前端**: Qt 6.x (C++17)
- **后端**: C++ + Python 3.8+
- **AI服务**: OpenAI API
- **数据存储**: JSON文件
- **网络通信**: Qt Network

## 🏗️ 核心类架构

### 主要类层次结构

```
QApplication
└── MainWindow
    ├── HomePage
    ├── CalendarWidget
    ├── EventWidget
    ├── ListViewWidget
    ├── Settings
    ├── AchievementsPanel
    ├── History
    ├── ShowConflicts
    ├── InExport
    ├── LogInSignUp
    ├── NewWXEventShow
    └── ReminderManager
```

### 核心组件

#### MainWindow
主窗口类，负责整体界面管理和导航。

```cpp
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleLoginSuccess();
    void onHomeClicked();
    void onCalendarClicked();
    // ... 其他槽函数

private:
    void initializePages();
    void loadUserSettings();
    bool checkAuth();
    // ... 私有方法
};
```

#### API
核心API类，提供事件管理的所有接口。

```cpp
class API
{
public:
    // 事件管理
    static int AddEvent(QJsonObject MyEvent);
    static bool DeleteEvent(QString id);
    static bool FinishEvent(QJsonObject MyEvent);
    
    // 事件查询
    static QJsonArray RetrieveAllEvent();
    static QJsonArray RetrieveAllFinishedEvent();
    static QJsonArray RetrieveAllDeletedEvent();
    
    // 冲突检测
    static bool IsEventConfliction(QDateTime StartTime, QDateTime EndTime);
    
    // 工具方法
    static bool IfEventLegal(QJsonObject MyEvent);
    static QString formatEventDate(const QJsonObject &event);
    static QColor getPriorityColor(int priority);
    static QString change_priority(const int i);
};
```

## 🔌 API接口文档

### 事件管理接口

#### 添加事件
```cpp
static int AddEvent(QJsonObject MyEvent);
```

**参数**:
- `MyEvent`: 事件JSON对象，包含以下字段：
  - `title`: 事件标题 (QString)
  - `startTime`: 开始时间 (QString, ISO格式)
  - `endTime`: 结束时间 (QString, ISO格式)
  - `description`: 事件描述 (QString, 可选)
  - `priority`: 优先级 (int, 0-4)
  - `url`: 相关链接 (QString, 可选)

**返回值**:
- `0`: 成功
- `-1`: 参数错误
- `-2`: 时间冲突
- `-3`: 数据库错误

**示例**:
```cpp
QJsonObject event;
event["title"] = "团队会议";
event["startTime"] = "2024-12-20T14:00:00";
event["endTime"] = "2024-12-20T15:00:00";
event["description"] = "讨论项目进度";
event["priority"] = 1;
event["url"] = "https://meet.example.com";

int result = API::AddEvent(event);
```

#### 删除事件
```cpp
static bool DeleteEvent(QString id);
```

**参数**:
- `id`: 事件唯一标识符

**返回值**:
- `true`: 删除成功
- `false`: 删除失败

#### 完成事件
```cpp
static bool FinishEvent(QJsonObject MyEvent);
```

**参数**:
- `MyEvent`: 要完成的事件对象

**返回值**:
- `true`: 完成成功
- `false`: 完成失败

### 事件查询接口

#### 查询所有事件
```cpp
static QJsonArray RetrieveAllEvent();
```

**返回值**:
- `QJsonArray`: 所有未删除事件的数组

#### 查询已完成事件
```cpp
static QJsonArray RetrieveAllFinishedEvent();
```

**返回值**:
- `QJsonArray`: 所有已完成事件的数组

#### 查询已删除事件
```cpp
static QJsonArray RetrieveAllDeletedEvent();
```

**返回值**:
- `QJsonArray`: 所有已删除事件的数组

#### 按日期查询事件
```cpp
static std::function<QJsonArray()> RetrieveFromDay(const QDate& today);
```

**参数**:
- `today`: 查询日期

**返回值**:
- `std::function<QJsonArray()>`: 返回查询函数

**示例**:
```cpp
QDate date(2024, 12, 20);
auto queryFunc = API::RetrieveFromDay(date);
QJsonArray events = queryFunc();
```

### 冲突检测接口

#### 检测时间冲突
```cpp
static bool IsEventConfliction(QDateTime StartTime, QDateTime EndTime);
```

**参数**:
- `StartTime`: 开始时间
- `EndTime`: 结束时间

**返回值**:
- `true`: 存在冲突
- `false`: 无冲突

### 工具接口

#### 事件合法性验证
```cpp
static bool IfEventLegal(QJsonObject MyEvent);
```

**参数**:
- `MyEvent`: 事件对象

**返回值**:
- `true`: 事件合法
- `false`: 事件不合法

#### 格式化事件日期
```cpp
static QString formatEventDate(const QJsonObject &event);
```

**参数**:
- `event`: 事件对象

**返回值**:
- `QString`: 格式化后的日期字符串 (格式: DD-MM-DD)

#### 获取优先级颜色
```cpp
static QColor getPriorityColor(int priority);
```

**参数**:
- `priority`: 优先级 (0-4)

**返回值**:
- `QColor`: 对应的颜色对象

**颜色映射**:
- 0: 红色 (#FF0000) - 非常重要
- 1: 橙红色 (#D94000) - 重要
- 2: 橙色 (#B38000) - 一般
- 3: 黄绿色 (#8CBF00) - 不重要
- 4: 绿色 (#66FF00) - 非常不重要

#### 优先级转中文
```cpp
static QString change_priority(const int i);
```

**参数**:
- `i`: 优先级数字

**返回值**:
- `QString`: 中文描述

## 📊 数据结构

### 事件数据结构

```json
{
  "id": "unique_event_id",
  "title": "事件标题",
  "startTime": "2024-12-20T14:00:00",
  "endTime": "2024-12-20T15:00:00",
  "description": "事件描述",
  "priority": 1,
  "url": "https://example.com",
  "createdAt": "2024-12-20T10:00:00",
  "updatedAt": "2024-12-20T10:00:00",
  "isFinished": false,
  "isDeleted": false
}
```

### 用户设置数据结构

```json
{
  "user_name": "用户名",
  "remind15MinBefore": true,
  "remindAtStart": true,
  "closeToExit": false,
  "wechatRunning": false,
  "wechatUserlist": ["联系人1", "联系人2"]
}
```

### 成就数据结构

```json
{
  "id": "achievement_id",
  "name": "成就名称",
  "description": "成就描述",
  "icon": "achievement_icon.png",
  "isUnlocked": false,
  "progress": 0,
  "maxProgress": 100,
  "unlockDate": null
}
```

## 🔄 事件系统

### 信号槽机制

#### 主要信号
```cpp
// 事件相关信号
void eventAdded(const QJsonObject &event);
void eventUpdated(const QJsonObject &event);
void eventDeleted(const QString &id);
void eventFinished(const QJsonObject &event);

// 冲突检测信号
void conflictDetected(const QJsonArray &conflicts);

// 提醒信号
void reminderTriggered(const QJsonObject &event);

// 微信消息信号
void wxEventReceive(const QJsonObject &event);
```

#### 连接示例
```cpp
// 连接事件添加信号
connect(api, &API::eventAdded, this, &MainWindow::onEventAdded);

// 连接冲突检测信号
connect(api, &API::conflictDetected, this, &MainWindow::onConflictDetected);
```

## 🐍 Python集成

### PythonAPI类

```cpp
class PythonAPI : public QObject
{
    Q_OBJECT
public:
    explicit PythonAPI(QObject *parent = nullptr);
    ~PythonAPI();

public slots:
    void StartWxre();
    void EndWxre();

signals:
    void wxEventReceive(const QJsonObject &event);

private:
    QProcess *pythonProcess;
    void setupPythonProcess();
};
```

### 微信监听脚本接口

#### 启动监听
```cpp
void StartWxre();
```

启动Python微信监听进程，开始监听指定联系人的消息。

#### 停止监听
```cpp
void EndWxre();
```

停止Python微信监听进程。

#### 消息接收
```cpp
void wxEventReceive(const QJsonObject &event);
```

接收来自Python脚本的解析后事件数据。

### Python脚本参数

微信监听脚本 (`py/wxevent.py`) 支持以下命令行参数：

```bash
python wxevent.py --api_key YOUR_API_KEY --contacts 联系人1 联系人2
```

**参数说明**:
- `--api_key`: OpenAI API密钥 (必需)
- `--dify_key`: Dify API密钥 (可选)
- `--send_greeting`: 是否发送检测信息 (0/1, 默认1)
- `--save_pic`: 是否保存图片 (0/1, 默认1)
- `--save_file`: 是否保存文件 (0/1, 默认1)
- `--save_voice`: 是否保存语音 (0/1, 默认1)
- `--contacts`: 监听联系人列表 (必需)

## 💾 数据库设计

### 文件存储结构

```
用户数据目录/
├── schedules/           # 事件数据
│   ├── events.json     # 当前事件
│   ├── finished.json   # 已完成事件
│   └── deleted.json    # 已删除事件
├── settings.json       # 用户设置
├── achievements.json   # 成就数据
└── history.json        # 历史记录
```

### 数据文件格式

#### events.json
```json
{
  "events": [
    {
      "id": "event_1",
      "title": "会议",
      "startTime": "2024-12-20T14:00:00",
      "endTime": "2024-12-20T15:00:00",
      "description": "团队会议",
      "priority": 1,
      "url": "",
      "createdAt": "2024-12-20T10:00:00",
      "updatedAt": "2024-12-20T10:00:00",
      "isFinished": false,
      "isDeleted": false
    }
  ]
}
```

#### settings.json
```json
{
  "user_name": "用户名",
  "remind15MinBefore": true,
  "remindAtStart": true,
  "closeToExit": false,
  "wechatRunning": false,
  "wechatUserlist": ["联系人1", "联系人2"]
}
```

## ⚠️ 错误处理

### 错误代码定义

```cpp
enum ErrorCode {
    SUCCESS = 0,
    INVALID_PARAMETER = -1,
    TIME_CONFLICT = -2,
    DATABASE_ERROR = -3,
    NETWORK_ERROR = -4,
    API_ERROR = -5,
    PERMISSION_DENIED = -6
};
```

### 异常处理

#### API异常
```cpp
try {
    int result = API::AddEvent(event);
    if (result != 0) {
        // 处理错误
        handleError(result);
    }
} catch (const std::exception& e) {
    qDebug() << "API异常:" << e.what();
}
```

#### 网络异常
```cpp
void handleNetworkError(QNetworkReply::NetworkError error) {
    switch (error) {
        case QNetworkReply::ConnectionRefusedError:
            // 处理连接被拒绝
            break;
        case QNetworkReply::TimeoutError:
            // 处理超时
            break;
        default:
            // 处理其他网络错误
            break;
    }
}
```

## 🚀 性能优化

### 内存管理

#### 对象生命周期
- 使用智能指针管理动态分配的对象
- 及时释放不再使用的资源
- 避免内存泄漏

#### 缓存策略
```cpp
// 事件缓存
QHash<QString, QJsonObject> eventCache;

// 查询缓存
QHash<QDate, QJsonArray> dateCache;
```

### 查询优化

#### 索引优化
- 为频繁查询的字段建立索引
- 使用哈希表加速查找
- 分页加载大量数据

#### 异步处理
```cpp
// 异步加载事件
QFuture<QJsonArray> future = QtConcurrent::run([=]() {
    return API::RetrieveAllEvent();
});

// 处理完成信号
connect(&future, &QFutureWatcher<QJsonArray>::finished, this, [=]() {
    QJsonArray events = future.result();
    updateEventList(events);
});
```

### UI优化

#### 虚拟化列表
- 对于大量数据使用虚拟化列表
- 只渲染可见的项目
- 减少内存占用

#### 延迟加载
```cpp
// 延迟加载图片
void loadImageAsync(const QString &path) {
    QFuture<QPixmap> future = QtConcurrent::run([=]() {
        return QPixmap(path);
    });
    
    connect(&future, &QFutureWatcher<QPixmap>::finished, this, [=]() {
        setPixmap(future.result());
    });
}
```

## 📝 开发指南

### 代码规范

#### 命名规范
- 类名：PascalCase (如 `MainWindow`)
- 方法名：camelCase (如 `addEvent`)
- 常量：UPPER_SNAKE_CASE (如 `MAX_EVENTS`)
- 私有成员：下划线前缀 (如 `_privateMember`)

#### 注释规范
```cpp
/**
 * @brief 添加新事件
 * @param event 事件对象
 * @return 操作结果码
 * @note 此方法会检查时间冲突
 */
static int AddEvent(QJsonObject event);
```

### 测试指南

#### 单元测试
```cpp
// 使用Qt Test框架
class APITest : public QObject
{
    Q_OBJECT

private slots:
    void testAddEvent();
    void testDeleteEvent();
    void testConflictDetection();
};
```

#### 集成测试
- 测试完整的用户流程
- 验证数据一致性
- 检查性能指标

---

**版本**: 1.0.0  
**最后更新**: 2024年12月  
**维护者**: 开发团队 