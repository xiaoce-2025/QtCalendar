#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "basicvariable.h"
#include "mymessagebox.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QPushButton>
#include <QColor>
#include <QMenu>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    this->resize(1200, 675);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainStack = new QStackedWidget;
    mainLayout->addWidget(mainStack);

    // 去掉周围的边距
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setStatusBar(nullptr);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    initializePages();

    // 初始显示登录页面
    mainStack->setCurrentWidget(loginPage);

    // 创建系统托盘图标
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/test.ico.png"));

    // 创建托盘菜单
    QMenu *trayMenu = new QMenu(this);
    trayMenu->setStyleSheet(
        "QMenu {"
        "   background-color: #ffffff;"       // 背景色
        "   border: 1px solid #e0e0e0;"      // 边框
        "   border-radius: 6px;"              // 圆角
        "   padding: 8px 0;"                  // 内边距
        "   font: 14px '微软雅黑';"           // 字体
        "}"
        "QMenu::item {"
        "   height: 32px;"                   // 项高度
        "   padding: 0 24px 0 32px;"         // 项内边距
        "   color: #333333;"                 // 文字颜色
        "   background-color: transparent;"  // 背景透明
        "}"
        "QMenu::item:selected {"
        "   background-color: #f0f0f0;"      // 选中背景
        "   border-radius: 4px;"             // 选中圆角
        "}"
        "QMenu::icon {"
        "   left: 8px;"                      // 图标位置
        "}"
        );

    this->setStyleSheet(
        // 主窗口样式
        "QMainWindow {"
        "   background: #ecf0f1;"
        "}"

        // 内容区域样式
        "#contentArea {"
        "   background: #ffffff;"
        "   border-left: 1px solid #bdc3c7;"
        "}"

        // 标题样式
        "QLabel#titleLabel {"
        "   color: #2c3e50;"
        "   font: bold 24px '微软雅黑';"
        "   margin: 15px;"
        "}"
        );

    QAction *viewCalendarAction = trayMenu->addAction("查看日历");
    QAction *addEventAction = trayMenu->addAction("添加日程");
    trayMenu->addSeparator();
    QAction *restoreAction = trayMenu->addAction("恢复窗口");
    trayMenu->addSeparator();
    QAction *quitAction = trayMenu->addAction("退出系统");

    connect(viewCalendarAction, &QAction::triggered, this, [=]{
        showNormal();
        activateWindow();
        onCalendarClicked();
    });
    connect(addEventAction, &QAction::triggered, this, [=]{
        showNormal();
        activateWindow();
        onEventClicked();
    });
    connect(restoreAction, &QAction::triggered, this, &MainWindow::showNormal);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    // 连接托盘图标激活信号
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);

    wxre = nullptr;
}


void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) { // 左键点击
        showNormal(); // 恢复窗口
        activateWindow(); // 激活窗口
    }
}



void MainWindow::initializePages()
{
    // 主容器（水平布局：导航栏 + 内容区）
    mainContainer = new QWidget;
    QHBoxLayout *containerLayout = new QHBoxLayout(mainContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    // 创建左侧导航栏
    sidebar = new QWidget;
    sidebar->setFixedWidth(200);
    sidebar->setObjectName("sidebar");
    sidebar->setStyleSheet(
        "#sidebar { background: #2c3e50; }"
        "QLabel { color: #ecf0f1; }"
        );

    sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 15, 0, 15);
    sidebarLayout->setSpacing(5);

    // 创建导航按钮
    createNavigationButton(tr("首页"), "homepage", SLOT(onHomeClicked()));
    createNavigationButton(tr("日历"), "calendar", SLOT(onCalendarClicked()));
    createNavigationButton(tr("添加日程"), "event_add", SLOT(onEventClicked()));
    createNavigationButton(tr("全部任务"), "tasks", SLOT(onTaskClicked()));
    //createNavigationButton(tr("查看冲突日程"), "showconflicts", SLOT(onConflictsClicked()));
    createNavigationButton(tr("历史记录"), "history", SLOT(onHistoryClicked()));
    createNavigationButton(tr("成就"), "achievements", SLOT(onAchievementClicked()));
    //createNavigationButton(tr("导入导出"), "ImExport", SLOT(onInExportClicked()));
    createNavigationButton(tr("设置"), "settings", SLOT(onSettingsClicked()));

    sidebarLayout->addStretch(1); // 填充中间空间

    // 用户信息区域
    userInfoWidget = new QWidget;
    QVBoxLayout *userLayout = new QVBoxLayout(userInfoWidget);
    userLayout->setContentsMargins(10, 10, 10, 10);

    userNameLabel = new QLabel(BasicVariable::user_name.isEmpty() ?
                                   tr("未登录") : BasicVariable::user_name);
    userNameLabel->setStyleSheet("font: 12px '微软雅黑';");

    loginBtn = new QPushButton(BasicVariable::IfLog ? tr("退出登录") : tr("登录"));
    loginBtn->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #ecf0f1;"
        "   text-align: center;"
        "   padding: 8px;"
        "   border: 1px solid #3498db;"
        "   border-radius: 4px;"
        "   outline: none;"
        "}"
        "QPushButton:hover { background: rgba(52, 152, 219, 0.3); }"
        );
    connect(loginBtn,&QPushButton::clicked,this,&MainWindow::onLoginClicked);

    userLayout->addWidget(userNameLabel);
    userLayout->addWidget(loginBtn);
    userLayout->addStretch();

    sidebarLayout->addWidget(userInfoWidget);

    // 右侧内容区
    contentStack = new QStackedWidget;
    contentStack->setObjectName("contentArea");

    // 添加所有功能页面
    /*
    calendarPage = new CalendarWidget;
    eventPage = new EventWidget;
    taskPage = new ListViewWidget;
    achievePage = new achievementspanel;

    contentStack->addWidget(calendarPage);
    contentStack->addWidget(eventPage);
    contentStack->addWidget(taskPage);
    contentStack->addWidget(achievePage);
    */
    // 组合布局
    containerLayout->addWidget(sidebar);
    containerLayout->addWidget(contentStack, 1);
    mainContainer->setLayout(containerLayout);

    // 初始显示登录页面
    // contentStack->setCurrentIndex(0);


    // 含登录页面的页面汇总
    loginPage = new LogInSignUp;
    connect(loginPage, &LogInSignUp::loginSuccess, this, &MainWindow::handleLoginSuccess);
    mainStack->addWidget(loginPage);
    mainStack->addWidget(mainContainer);

    // 微信监听展示窗口
    createWxMessagePage();
    mainStack->addWidget(wxMessagePage);

    // 调试用
    //mainContainer->setStyleSheet("background-color: red;");
}

// 左侧导航栏
void MainWindow::createNavigationButton(const QString &text,
                                        const QString &iconName,
                                        const char *slot)
{
    QPushButton *btn = createNavButton(text, iconName);
    sidebarLayout->addWidget(btn);
    connect(btn, SIGNAL(clicked()), this, slot);
}

QPushButton* MainWindow::createNavButton(const QString &text,
                                         const QString &iconName)
{
    QPushButton *btn = new QPushButton(text);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setIcon(QIcon(":/icons/" + iconName + ".png"));
    btn->setIconSize(QSize(24, 24));
    btn->setStyleSheet(
        "QPushButton {"
        "   text-align: left;"
        "   padding: 12px 20px;"
        "   color: #ecf0f1;"
        "   background: transparent;"
        "   border: none;"
        "   font: 14px '微软雅黑';"
        "   outline: none;"
        "}"
        "QPushButton:hover {"
        "   background: #34495e;"
        "   border-left: 3px solid #3498db;"
        "}"
        "QPushButton:pressed {"
        "   background: #1abc9c;"
        "}"
        );
    return btn;
}


/*
QPushButton* MainWindow::createStyledButton(const QString &text, const QString &color)
{
    QPushButton *btn = new QPushButton(text);
    btn->setMinimumSize(240, 80);

    QColor baseColor(color);
    QString darkerColor = baseColor.darker(110).name();// 亮度降低10%
    QString darkestColor = baseColor.darker(130).name();// 亮度降低30%

    btn->setStyleSheet(QString(
                           "QPushButton {"
                           "background-color: %1;"
                           "color: #2C3E50;"
                           "font: bold 18px '微软雅黑';"
                           "border-radius: 12px;"
                           "padding: 12px;"
                           "border: 2px solid %2;"
                           "}"
                           "QPushButton:hover {"
                           "background-color: %2;"
                           "transform: scale(1.05);"
                           "}"
                           "QPushButton:pressed {"
                           "background-color: %3;"
                           "}"
                           ).arg(color, darkerColor, darkestColor));

    return btn;
}
*/
/*
QPushButton* MainWindow::createPlaceholderButton()
{
    QPushButton *placeholder = new QPushButton;
    placeholder->setMinimumSize(240, 80);
    placeholder->setEnabled(false);
    placeholder->setStyleSheet(
        "QPushButton {"
        "background-color: #ecf0f1;"
        "border: 2px dashed #bdc3c7;"
        "border-radius: 12px;"
        "}"
        );
    return placeholder;
}
*/
/*
void MainWindow::handleBackRequest()
{
    mainStack->setCurrentWidget(mainContainer);
    contentStack->setCurrentIndex(0);
    resize(1200, 675);
    showNormal();
}
*/


bool MainWindow::checkAuth()
{
    if (!BasicVariable::IfLog) {
        MyMessageBox choose(nullptr,tr("提示"),tr("请先登录！"),QMessageBox::Question,QMessageBox::Yes | QMessageBox::No);
        choose.button(QMessageBox::Yes)->setText(tr("去登录"));
        choose.button(QMessageBox::No)->setText(tr("取消"));
        int temp_15=choose.exec();
        if (temp_15 == QMessageBox::Yes){
            mainStack->setCurrentWidget(loginPage);
        }
        return false;
    }
    return true;
}


void MainWindow::clearContentWidget(){
    while(contentStack->currentWidget()){
        QWidget *p = contentStack->currentWidget();
        contentStack->removeWidget(p);
        delete p;
    }
    return;
}

template<class T>
T* MainWindow::changeContent(){
    if (!checkAuth()) return nullptr;
    clearContentWidget();
    T* p = new T;
    contentStack->addWidget(p);
    contentStack->setCurrentWidget(p);
    this->resize(1200, 675);
    return p;
}

// 首页
void MainWindow::onHomeClicked(){
    HomePage *p = changeContent<HomePage>();
    if (p == nullptr)   return;
    return;
}

// 成就视图
void MainWindow::onAchievementClicked(){
    achievementspanel *p = changeContent<achievementspanel>();
    if (p == nullptr)   return;
    p->init(API::RetrieveAllFinishedEvent().size());
    return;
}

// 月日历视图
void MainWindow::onCalendarClicked()
{
    CalendarWidget *p = changeContent<CalendarWidget>();
    if (p == nullptr)   return;
    //calendarPage->setDay(QDate::fromString("1919-05-17"));
    p->setDay(QDate::currentDate());
    p->MainWindow_Refresh();
    p->mustnotback();
    return ;
}

// 添加事件视图
void MainWindow::onEventClicked()
{
    EventWidget *p = changeContent<EventWidget>();
    if (p == nullptr)   return;
    p->init(QJsonObject());
    connect(p,&EventWidget::finish,this,&MainWindow::handleEditorBackRequest);
    return ;
}

// 事件列表视图
void MainWindow::onTaskClicked()
{
    ListViewWidget *p = changeContent<ListViewWidget>();
    if (p == nullptr)   return;
    p->setRange(API::RetrieveAllEvent,QDate());
    p->mustnotback();
    return;
}

// 历史记录视图
void MainWindow::onHistoryClicked()
{
    History *p = changeContent<History>();
    if (p == nullptr)   return;
    qDebug()<<"尝试加载历史记录";
    return;
}

// 冲突日程视图
void MainWindow::onConflictsClicked()
{
    ShowConflicts *p = changeContent<ShowConflicts>();
    if (p == nullptr)   return;
    qDebug()<<"尝试加载冲突日程";
    return;
}

// 导入导出视图
void MainWindow::onInExportClicked()
{
    InExport *p = changeContent<InExport>();
    if (p == nullptr)   return;
    qDebug()<<"尝试加载导入导出模块";
    return;
}

// 设置视图
void MainWindow::onSettingsClicked(){
    Settings *p = changeContent<Settings>();
    if (p == nullptr)   return;
    // connect(settingPage, &Settings::backRequest, this, &MainWindow::handleSettingsBackRequest);
    connect(p, &Settings::startWx, this, &MainWindow::StartWxre);
    connect(p, &Settings::endWx, this, &MainWindow::EndWxre);
    return;
}

void MainWindow::onLoginClicked()
{
    if (BasicVariable::IfLog) {
        MyMessageBox choose(nullptr,tr("提示"),tr("你确认要退出登录吗？"),QMessageBox::Question,QMessageBox::Yes | QMessageBox::No);
        choose.button(QMessageBox::Yes)->setText(tr("退出登录"));
        choose.button(QMessageBox::No)->setText(tr("取消"));
        int temp_15=choose.exec();
        if (temp_15 == QMessageBox::Yes){
            // 重新构建loginsignup的界面
            clearContentWidget();
            mainStack->removeWidget(loginPage);
            loginPage->deleteLater();
            loginPage = new LogInSignUp(this);
            mainStack->insertWidget(0, loginPage);
            connect(loginPage, &LogInSignUp::loginSuccess, this, &MainWindow::handleLoginSuccess);


            BasicVariable::IfLog = false;
            BasicVariable::user_name.clear();
            loginBtn->setText(tr("登录"));
            mainStack->setCurrentWidget(loginPage);

            // 退出登录时自动退出微信监听
            if (wxre != nullptr){
                wxre->stopwxautoScript();
                BasicVariable::wechatRunning = false;
            }
            delete reminder;
        }
    } else {
        mainStack->setCurrentWidget(loginPage);
    }
}

void MainWindow::handleLoginSuccess()
{
    BasicVariable::IfLog = true;
    loginBtn->setText(tr("退出登录"));
    userNameLabel->setText(BasicVariable::user_name);

    // 初始显示首页
    onHomeClicked();

    userNameLabel->setText(BasicVariable::user_name);
    userNameLabel->setStyleSheet("QLabel {"
                                 "color: #34495e;"
                                 "font: 14px '微软雅黑';"
                                 "font-weight: bold;"
                                 "margin-right: 15px;"
                                 "}");

    loadUserSettings();

    // 启动微信监听进程
    if (UseWxRe){
        wxre = new PythonAPI(this);
        connect(wxre, &PythonAPI::wxscheduleReceived,
                this, &MainWindow::wxEventReceive);
        wxre->startwxautoScript();
        BasicVariable::wechatRunning = true;
    }

    // 启动提醒监测
    reminder = new ReminderManager(this);
    reminder->startMonitoring();

    mainStack->setCurrentWidget(mainContainer);
}


void MainWindow::loadUserSettings() {
    // 构建用户配置文件路径
    QString configPath = "data/users/" + BasicVariable::user_name + "/config.ini";
    QSettings userSettings(configPath, QSettings::IniFormat);

    // 读取微信监听设置
    userSettings.beginGroup("Wechat");
    QString startcommand = userSettings.value("pythonParams", QString()).toString();
    QString allusera = userSettings.value("watchedUsers", QString()).toString();
    UseWxRe = userSettings.value("autoStart", false).toBool();
    bool sendInfo = userSettings.value("sendInfo", true).toBool();
    bool listenImages = userSettings.value("listenImages", true).toBool();
    bool listenFiles = userSettings.value("listenFiles", true).toBool();
    bool listenVoice = userSettings.value("listenVoice", true).toBool();
    userSettings.endGroup();
    qDebug()<<startcommand<<" "<<allusera;

    // 读取窗口行为设置
    userSettings.beginGroup("WindowBehavior");
    BasicVariable::closeToExit = userSettings.value("closeToExit", false).toBool();
    userSettings.endGroup();

    // api信息
    userSettings.beginGroup("APIs");
    QString guiapi = userSettings.value("siliconFlowAPI", QString()).toString();
    QString difyapi = userSettings.value("difyAPI", QString()).toString();
    userSettings.endGroup();

    // 读取提醒设置
    userSettings.beginGroup("Reminders");
    bool remind15Min = userSettings.value("remind15MinBefore", true).toBool();
    bool remindAtStart = userSettings.value("remindAtStart", true).toBool();
    userSettings.endGroup();

    // 应用提醒设置
    BasicVariable::remind15MinBefore = remind15Min;
    BasicVariable::remindAtStart = remindAtStart;

    // 微信监听的启动参数设置
    BasicVariable::params=QStringList();
    BasicVariable::startPython = startcommand;
    BasicVariable::wechatUserlist = allusera.split(";", Qt::SkipEmptyParts);
    BasicVariable::params << "--api_key" << guiapi
                          << "--dify_key" << difyapi
                          << "--send_greeting" << (sendInfo?"1":"0")
                          << "--save_pic" << (listenImages?"1":"0")
                          << "--save_file" << (listenFiles?"1":"0")
                          << "--save_voice" << (listenVoice?"1":"0")
                          << "--contacts";
    for (QString& user : BasicVariable::wechatUserlist) {
        user = user.trimmed();
        BasicVariable::params << user;
    }
}


void MainWindow::StartWxre(){
    wxre = new PythonAPI(this);
    connect(wxre, &PythonAPI::wxscheduleReceived,
            this, &MainWindow::wxEventReceive);
    wxre->startwxautoScript();
    BasicVariable::wechatRunning = true;
}


void MainWindow::closeEvent(QCloseEvent *event){
    if (!BasicVariable::closeToExit){
        event->ignore();
        hide();
        return;
    }
    MyMessageBox choose(nullptr,tr("提示"),tr("您正在关闭日程管理系统\n确认要退出吗？"),QMessageBox::Question,QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    choose.button(QMessageBox::Yes)->setText(tr("退出系统"));
    choose.button(QMessageBox::No)->setText(tr("最小化"));
    choose.button(QMessageBox::Cancel)->setText(tr("返回"));
    int temp_15=choose.exec();
    if (temp_15 == QMessageBox::Cancel){
        event->ignore();
        return;
    }
    else if (temp_15 == QMessageBox::No){
        event->ignore();
        hide();
        trayIcon->show();
        return;
    }
    else{
        if (wxre != nullptr){
            wxre->stopwxautoScript();
            BasicVariable::wechatRunning = false;
        }
        event->accept();
        exit(101);
        return;
    }
}


void MainWindow::EndWxre(){
    if (wxre != nullptr){
        wxre->stopwxautoScript();
        BasicVariable::wechatRunning = false;
    }
}

// 微信监听初始化
void MainWindow::createWxMessagePage() {
    wxMessagePage = new NewWXEventShow();
    // 连接返回信号
    connect(wxMessagePage, &NewWXEventShow::backRequested,
            this, &MainWindow::handleWechatBackRequest);
}


// 微信监听处理
void MainWindow::wxEventReceive(const QJsonObject &event){
    qDebug()<<"收到微信监听处理结果:"<<event;
    // 切换到消息页面
    if(mainStack->currentWidget() != wxMessagePage) {
        mainStack->setCurrentWidget(wxMessagePage);
    }

    // 添加消息条目（线程安全方式）
    QMetaObject::invokeMethod(wxMessagePage, [this, event](){
        wxMessagePage->addMessage(event);
    }, Qt::QueuedConnection);
}


void MainWindow::handleWechatBackRequest()
{
    mainStack->setCurrentWidget(mainContainer);
    resize(1200, 675);
    showNormal();
    delete wxMessagePage;
    createWxMessagePage();
    mainStack->addWidget(wxMessagePage);
}

// 添加事件结束后操作提示
void MainWindow::handleEditorBackRequest(bool success){
    int temp_15 = QMessageBox::No;
    if (success){
        MyMessageBox choose(nullptr,tr("提示"),tr("事件保存成功！"),QMessageBox::Information,QMessageBox::Yes | QMessageBox::No);
        choose.button(QMessageBox::Yes)->setText(tr("继续添加"));
        choose.button(QMessageBox::No)->setText(tr("查看日历"));
        temp_15=choose.exec();
    }
    else{
        MyMessageBox choose(nullptr,tr("提示"),tr("已取消添加事件"),QMessageBox::Critical,QMessageBox::Yes | QMessageBox::No);
        choose.button(QMessageBox::Yes)->setText(tr("重新添加"));
        choose.button(QMessageBox::No)->setText(tr("查看日历"));
        temp_15=choose.exec();
    }
    QTimer::singleShot(0, this, [this, temp_15](){
        if (temp_15 == QMessageBox::Yes){
            onEventClicked();
        }
        else{
            onCalendarClicked();
        }
    });
    return ;
}

/*
void MainWindow::handleSettingsBackRequest(){
    contentStack->setCurrentIndex(0);
    contentStack->removeWidget(settingPage);
    resize(1200, 675);
    loadUserSettings();
    showNormal();
    delete settingPage;
}
*/

MainWindow::~MainWindow()
{
    delete ui;
}
