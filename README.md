# 智能日历管理系统

一个基于Qt开发的现代化智能日历和事件管理应用程序，集成了微信消息监听、AI智能解析、成就系统等创新功能。

## 🚀 项目特性

### 核心功能
- **智能日历管理** - 可视化的日历界面，支持事件创建、编辑、删除
- **事件冲突检测** - 自动检测时间冲突，避免日程安排冲突
- **多优先级支持** - 5级重要性分类，颜色编码显示
- **提醒系统** - 可配置的事件提醒（开始前15分钟、开始时）
- **历史记录** - 完整的事件历史追踪和管理

### 创新功能
- **微信集成** - 实时监听微信消息，自动解析日程信息
- **AI智能解析** - 基于deepseek和千问大模型的智能文本和图片解析
- **成就系统** - 用户行为激励和成就解锁
- **系统托盘** - 后台运行，最小化到系统托盘

## 🛠️ 技术栈

- **前端框架**: Qt 6.x (C++)
- **UI设计**: Qt Widgets + QML
- **AI集成**: OpenAI API (DeepSeek-V3, Qwen2.5-VL)
- **微信监听**: wxauto Python库
- **数据存储**: JSON格式本地存储
- **网络通信**: Qt Network模块

## 📋 系统要求

- **操作系统**: Windows 10/11
- **Qt版本**: 6.0或更高版本
- **Python**: 3.8+ (用于微信监听功能)
- **内存**: 至少4GB RAM
- **存储**: 至少100MB可用空间

## 🔧 安装说明

### 1. 环境准备
```bash
# 安装Qt 6.x
# 下载并安装Qt Creator和Qt库

# 安装Python依赖
cd py/
pip install -r requirements.txt
```

### 2. 编译项目
```bash
# 使用Qt Creator打开homework.pro文件
# 或者使用命令行编译
qmake homework.pro
make
```

### 3. 配置API密钥
在`py/wxevent.py`中配置您的API密钥：
- OpenAI API密钥
- Dify API密钥（可选）

### 4. 运行程序
```bash
# 直接运行可执行文件
./homework.exe
```

## 📁 项目结构

```
test2/
├── main.cpp                 # 程序入口
├── mainwindow.cpp/h         # 主窗口管理
├── calendarwidget.cpp/h     # 日历组件
├── eventwidget.cpp/h        # 事件管理组件
├── api.cpp/h               # 核心API接口
├── pythonapi.cpp/h         # Python集成接口
├── py/                     # Python脚本目录
│   ├── wxevent.py         # 微信监听脚本
│   └── requirements.txt   # Python依赖
├── pictures/              # 资源文件
└── *.ui                   # Qt界面文件
```

## 🎯 主要功能模块

### 日历管理 (CalendarWidget)
- 月视图日历显示
- 事件可视化标记
- 日期导航和快速跳转

### 事件管理 (EventWidget)
- 事件创建和编辑
- 优先级设置
- 时间冲突检测

### 微信集成 (PythonAPI)
- 实时消息监听
- 智能文本解析
- 图片OCR识别
- 文档解析

### 成就系统 (AchievementsPanel)
- 用户行为追踪
- 成就解锁机制
- 进度可视化

## 🔐 配置说明

### 微信监听配置
#### 微信监听功能支持的微信版本为3.9.x - 3.12.x 中的任意windows版本
#### ※ 由于微信近期版本更新，目前暂不支持4.x的微信版本
#### 开发者使用时，请将PythonAPI.cpp中的文件路径修改为自己下载的文件路径！
在设置界面配置设置：
- 监听联系人列表
- API密钥配置
- 监听选项

### 提醒设置
在设置界面配置：
- 提前提醒时间
- 提醒方式
- 系统托盘设置

## 🤝 贡献指南

1. Fork 项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开 Pull Request

## 🙏 致谢

- 感谢助教和老师们的辛勤指导！

## 免责声明

**注意**: 使用微信监听功能时，请确保遵守相关法律法规和微信使用条款。

对于一切滥用微信监听功能所导致的后果，本程序及本程序开发者不承担一切责任。

