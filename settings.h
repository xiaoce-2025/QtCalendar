#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QJsonObject>
#include <QSettings>
#include "basicvariable.h"

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private:
    Ui::Settings *ui;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 微信监听相关槽函数
    void onWechatToggleClicked();
    void updateWechatControls(bool enabled);

    // 应用和保存设置
    void applySettings(bool tanchaung);
    void saveSettings();
    void cancelChanges();

    // API设置验证
    void validateAPIs();

    // 快捷键设置
    void changeAddEventShortcut();
    void changePrevEventShortcut();
    void changeNextEventShortcut();

signals:
    void backRequest();
    void startWx();
    void endWx();

private:
    void loadSettings();
    void setupConnections();
    void showRestartMessage();

    QSettings* appSettings;

    // 状态标志
    bool settingsChanged = false;

    // 当前快捷键设置
    QString addEventShortcut;
    QString prevEventShortcut;
    QString nextEventShortcut;
};

#endif // SETTINGS_H
