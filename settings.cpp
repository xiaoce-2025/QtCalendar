#include "settings.h"
#include "ui_settings.h"
#include <QMessageBox>
#include "basicvariable.h"
#include <QCloseEvent>
#include <QProcess>
#include <QShortcut>
#include <QDebug>

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings),
    appSettings(new QSettings("MyCompany", "MyApp", this))
{
    ui->setupUi(this);
    setWindowTitle("系统设置");

    // 构建用户特定配置文件路径
    QString configPath = "data/users/" + BasicVariable::user_name + "/config.ini";
    appSettings = new QSettings(configPath, QSettings::IniFormat, this);

    loadSettings();
    setupConnections();
    validateAPIs();
    updateWechatControls(!BasicVariable::wechatRunning);

    ui->pushButton_5->hide();
    if (!BasicVariable::isDeveloping){
        ui->plainTextEdit->hide();
        ui->label->hide();
    }
}

Settings::~Settings()
{
    delete ui;
}

void Settings::loadSettings()
{
    appSettings->beginGroup("Wechat");
    ui->plainTextEdit->setPlainText(appSettings->value("pythonParams", "").toString());
    ui->plainTextEdit_2->setPlainText(appSettings->value("watchedUsers", "").toString());
    ui->checkBox->setChecked(appSettings->value("autoStart", false).toBool());
    ui->checkBox_2->setChecked(appSettings->value("sendInfo", true).toBool());
    ui->checkBox_3->setChecked(appSettings->value("listenImages", true).toBool());
    ui->checkBox_4->setChecked(appSettings->value("listenFiles", true).toBool());
    ui->checkBox_5->setChecked(appSettings->value("listenVoice", true).toBool());
    appSettings->endGroup();

    appSettings->beginGroup("WindowBehavior");
    bool closeToExit = appSettings->value("closeToExit", false).toBool();
    ui->radioButton->setChecked(!closeToExit);
    ui->radioButton_2->setChecked(closeToExit);
    appSettings->endGroup();

    appSettings->beginGroup("APIs");
    ui->lineEdit->setText(appSettings->value("siliconFlowAPI", "").toString());
    ui->lineEdit_2->setText(appSettings->value("difyAPI", "").toString());
    appSettings->endGroup();

    appSettings->beginGroup("Reminders");
    ui->checkBox_6->setChecked(appSettings->value("remind15MinBefore", true).toBool());
    ui->checkBox_7->setChecked(appSettings->value("remindAtStart", true).toBool());
    appSettings->endGroup();

    appSettings->beginGroup("Shortcuts");
    addEventShortcut = appSettings->value("addEvent", "Ctrl+Alt+N").toString();
    prevEventShortcut = appSettings->value("prevEvent", "Page Up").toString();
    nextEventShortcut = appSettings->value("nextEvent", "Page Down").toString();
    ui->pushButton_2->setText(addEventShortcut);
    ui->pushButton_3->setText(prevEventShortcut);
    ui->pushButton_4->setText(nextEventShortcut);
    appSettings->endGroup();

    if (BasicVariable::wechatRunning){
        ui->pushButton->setText("停止微信监听");
    }
    else{
        ui->pushButton->setText("开启微信监听");
    }
    updateWechatControls(!BasicVariable::wechatRunning);
}

void Settings::setupConnections()
{
    // 微信监听按钮
    connect(ui->pushButton, &QPushButton::clicked, this, &Settings::onWechatToggleClicked);

    // API验证
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &Settings::validateAPIs);
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &Settings::validateAPIs);

    // 快捷键按钮
    connect(ui->pushButton_2, &QPushButton::clicked, this, &Settings::changeAddEventShortcut);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &Settings::changePrevEventShortcut);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &Settings::changeNextEventShortcut);

    // 底部按钮
    connect(ui->pushButton_5, &QPushButton::clicked, this, &Settings::cancelChanges);
    connect(ui->pushButton_6, &QPushButton::clicked, this, &Settings::applySettings);
    connect(ui->pushButton_7, &QPushButton::clicked, this, &Settings::saveSettings);

    // 监听状态变化
    connect(ui->checkBox, &QCheckBox::stateChanged, this, [this]{ settingsChanged = true; });
    connect(ui->checkBox_2, &QCheckBox::stateChanged, this, [this]{ settingsChanged = true; });
    connect(ui->checkBox_3, &QCheckBox::stateChanged, this, [this]{ settingsChanged = true; });
    connect(ui->checkBox_4, &QCheckBox::stateChanged, this, [this]{ settingsChanged = true; });
    connect(ui->checkBox_5, &QCheckBox::stateChanged, this, [this]{ settingsChanged = true; });
    connect(ui->radioButton, &QRadioButton::clicked, this, [this]{ settingsChanged = true; });
    connect(ui->radioButton_2, &QRadioButton::clicked, this, [this]{ settingsChanged = true; });
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, [this]{ settingsChanged = true; });
    connect(ui->plainTextEdit_2, &QPlainTextEdit::textChanged, this, [this]{ settingsChanged = true; });
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this]{ settingsChanged = true; });
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, [this]{ settingsChanged = true; });
    connect(ui->checkBox_6, &QCheckBox::stateChanged, this, [this]{ settingsChanged = true; });
    connect(ui->checkBox_7, &QCheckBox::stateChanged, this, [this]{ settingsChanged = true; });
}

void Settings::onWechatToggleClicked()
{
    applySettings(false);
    if (BasicVariable::wechatRunning) {
        // 停止监听
        emit endWx();
        BasicVariable::wechatRunning = false;
        ui->pushButton->setText("开启微信监听");
        QMessageBox::information(this, "成功", "微信监听已停止");
    } else {
        // 启动监听
        emit startWx();
        BasicVariable::wechatRunning = true;
        ui->pushButton->setText("停止微信监听");
    }
    updateWechatControls(!BasicVariable::wechatRunning);
    settingsChanged = true;
}

void Settings::updateWechatControls(bool enabled)
{
    ui->plainTextEdit->setEnabled(enabled);
    ui->plainTextEdit_2->setEnabled(enabled);
    ui->checkBox->setEnabled(enabled);
    ui->checkBox_2->setEnabled(enabled);
    ui->checkBox_3->setEnabled(enabled);
    ui->checkBox_4->setEnabled(enabled);
    ui->checkBox_5->setEnabled(enabled);
}

void Settings::validateAPIs()
{
    // 硅基流动API验证 (基本格式检查)
    bool siliconFlowValid = ui->lineEdit->text().startsWith("sk-") &&
                            ui->lineEdit->text().length() > 20;

    // Dify API验证 (基本格式检查)
    bool difyValid = ui->lineEdit_2->text().startsWith("app-") &&
                     ui->lineEdit_2->text().length() > 15;

    // 更新样式
    QString validStyle = "border: 1px solid green;";
    QString invalidStyle = "border: 1px solid red;";

    ui->lineEdit->setStyleSheet(siliconFlowValid ? validStyle : invalidStyle);
    ui->lineEdit_2->setStyleSheet(difyValid ? validStyle : invalidStyle);
}

void Settings::changeAddEventShortcut()
{
    // 实际应用中会有更复杂的快捷键设置对话框
    // 这里简化处理为默认快捷键
    QString newShortcut = "Ctrl+Alt+N";
    addEventShortcut = newShortcut;
    ui->pushButton_2->setText(newShortcut);
    settingsChanged = true;
}

void Settings::changePrevEventShortcut()
{
    QString newShortcut = "Page Up";
    prevEventShortcut = newShortcut;
    ui->pushButton_3->setText(newShortcut);
    settingsChanged = true;
}

void Settings::changeNextEventShortcut()
{
    QString newShortcut = "Page Down";
    nextEventShortcut = newShortcut;
    ui->pushButton_4->setText(newShortcut);
    settingsChanged = true;
}

void Settings::applySettings(bool tanchuang = true)
{
    if (!settingsChanged) return;

    // 保存到QSettings
    appSettings->beginGroup("Wechat");
    appSettings->setValue("pythonParams", ui->plainTextEdit->toPlainText());
    appSettings->setValue("watchedUsers", ui->plainTextEdit_2->toPlainText());
    appSettings->setValue("autoStart", ui->checkBox->isChecked());
    appSettings->setValue("sendInfo", ui->checkBox_2->isChecked());
    appSettings->setValue("listenImages", ui->checkBox_3->isChecked());
    appSettings->setValue("listenFiles", ui->checkBox_4->isChecked());
    appSettings->setValue("listenVoice", ui->checkBox_5->isChecked());
    appSettings->endGroup();

    appSettings->beginGroup("WindowBehavior");
    appSettings->setValue("closeToExit", ui->radioButton_2->isChecked());
    appSettings->endGroup();

    appSettings->beginGroup("APIs");
    appSettings->setValue("siliconFlowAPI", ui->lineEdit->text());
    appSettings->setValue("difyAPI", ui->lineEdit_2->text());
    appSettings->endGroup();

    appSettings->beginGroup("Reminders");
    appSettings->setValue("remind15MinBefore", ui->checkBox_6->isChecked());
    appSettings->setValue("remindAtStart", ui->checkBox_7->isChecked());
    appSettings->endGroup();

    appSettings->beginGroup("Shortcuts");
    appSettings->setValue("addEvent", addEventShortcut);
    appSettings->setValue("prevEvent", prevEventShortcut);
    appSettings->setValue("nextEvent", nextEventShortcut);
    appSettings->endGroup();

    appSettings->sync();
    if (tanchuang){
        QMessageBox::information(this, "设置应用", "设置已保存，部分设置需要重启应用生效");
    }
    settingsChanged = false;

    BasicVariable

    // 更新系统快捷键 (实际实现会更复杂)
    showRestartMessage();

    // 设置更新
    BasicVariable::closeToExit = ui->radioButton_2->isChecked();
    // 应用提醒设置
    BasicVariable::params = QStringList();
    BasicVariable::remind15MinBefore = ui->checkBox_6->isChecked();
    BasicVariable::remindAtStart = ui->checkBox_7->isChecked();

    // 微信监听的启动参数设置
    BasicVariable::startPython = ui->plainTextEdit->toPlainText();
    BasicVariable::wechatUserlist = ui->plainTextEdit_2->toPlainText().split(";", Qt::SkipEmptyParts);
    BasicVariable::params << "--api_key" << ui->lineEdit->text()
                          << "--dify_key" << ui->lineEdit_2->text()
                          << "--send_greeting" << (ui->checkBox_2->isChecked()?"1":"0")
                          << "--save_pic" << (ui->checkBox_3->isChecked()?"1":"0")
                          << "--save_file" << (ui->checkBox_4->isChecked()?"1":"0")
                          << "--save_voice" << (ui->checkBox_5->isChecked()?"1":"0")
                          << "--contacts";
    for (QString& user : BasicVariable::wechatUserlist) {
        user = user.trimmed();
        BasicVariable::params << user;
    }
}

void Settings::saveSettings()
{
    applySettings();
    emit backRequest();
}

void Settings::cancelChanges()
{
    if (settingsChanged) {
        int ret = QMessageBox::question(this, "取消更改",
                                        "有未保存的更改，确定要取消吗?",
                                        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            applySettings();
            emit backRequest();
        } else if (ret == QMessageBox::Discard) {
            emit backRequest();
        }
    } else {
        emit backRequest();
    }
}

void Settings::closeEvent(QCloseEvent *event)
{
    if (settingsChanged) {
        int ret = QMessageBox::question(this, "未保存更改",
                                        "设置已更改但未保存，是否保存?",
                                        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            applySettings();
            emit backRequest();
            event->accept();
        } else if (ret == QMessageBox::Discard) {
            emit backRequest();
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        emit backRequest();
        event->accept();
    }
}

void Settings::showRestartMessage()
{
    return;
}
