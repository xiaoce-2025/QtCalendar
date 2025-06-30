#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "homepage.h"
#include "calendarwidget.h"
#include "eventwidget.h"
#include "settings.h"
#include "listviewwidget.h"
#include "newwxeventshow.h"
#include "loginsignup.h"
#include "mymessagebox.h"
#include "history.h"
#include "achievementspanel.h"
#include "remindermanager.h"
#include "inexport.h"
#include "showconflicts.h"
#include "api.h"
#include <QLabel>
#include <QPushButton>
#include <QSystemTrayIcon>
#include "pythonapi.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleLoginSuccess();
    //void handleBackRequest();
    void handleWechatBackRequest();
    void handleEditorBackRequest(bool success);
    //void handleSettingsBackRequest();
    void onHomeClicked();
    void onCalendarClicked();
    void onEventClicked();
    void onTaskClicked();
    void onLoginClicked();
    void onSettingsClicked();
    void onAchievementClicked();
    void onHistoryClicked();
    void onConflictsClicked();
    void onInExportClicked();
    void StartWxre();
    void EndWxre();
    // 系统托盘点击
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::MainWindow *ui;
    bool UseWxRe = false;
    QStackedWidget *mainStack;
    LogInSignUp *loginPage;
    /*
    achievementspanel *achievePage;
    CalendarWidget *calendarPage;
    Settings *settingPage;
    EventWidget *eventPage;
    ListViewWidget *taskPage;
    */
    ReminderManager *reminder;

    void initializePages();
    void loadUserSettings();
    bool checkAuth();

    /*
    QPushButton* createStyledButton(const QString &text, const QString &color);
    QPushButton* createPlaceholderButton();
    */
    QPushButton *taskBtn;

    // 系统托盘图标
    QSystemTrayIcon *trayIcon;

protected:
    void closeEvent(QCloseEvent *);

public:
    PythonAPI *wxre;

private slots:
    void wxEventReceive(const QJsonObject &event);

private:
    NewWXEventShow* wxMessagePage = nullptr;
    void createWxMessagePage();

private:
    void initializeNavigation();
    void clearContentWidget();
    template<class T>
    T* changeContent();
    void createNavigationButton(const QString &text, const QString &iconName,
                                const char *slot);
    QPushButton* createNavButton(const QString &text, const QString &iconName);

    // 左侧导航栏组件
    QWidget *sidebar;
    QVBoxLayout *sidebarLayout;
    QStackedWidget *contentStack; // 右侧内容区
    QWidget *mainContainer;

    // 用户信息区域
    QWidget *userInfoWidget;
    QLabel *userNameLabel;
    QPushButton *loginBtn;

};
#endif // MAINWINDOW_H
