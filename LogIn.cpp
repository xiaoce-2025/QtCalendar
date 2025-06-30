#include "LogIn.h"
#include "ui_LogIn.h"
#include <QtCore/qdir.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsondocument.h>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QStandardPaths>
#include "basicvariable.h"
#include "api.h"
#include <QHash>

LogIn::LogIn(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogIn)
{
    ui->setupUi(this);
    zhanghao = ui->zhanghao;
    mima = ui->mima;

    // 获取用户主目录
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString filePath = homePath + "/account.json";

    qDebug() << "文件将保存在:" << filePath;

    QFile file(filePath);
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly)) {
            QJsonObject defaultData;
            defaultData["users"] = QJsonArray();
            file.write(QJsonDocument(defaultData).toJson());
            file.close();
            qDebug() << "已创建 account.json";
        } else {
            qDebug() << "错误：无法创建文件，可能没有权限";
        }
    } else {
        if (file.open(QIODevice::ReadOnly)) {
            qDebug() << "文件存在且可读，内容:" << file.readAll();
            file.close();
        } else {
            qDebug() << "错误：文件存在但无法读取";
        }
    }
    QString tu,tp;
    QFile file1("save_log");
    if (file1.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file1);

        stream>>tu>>tp;
        file1.close();
    } else {
        qDebug() << "Error: 无法打开账密临存文件";
    }
    if (!tu.isEmpty())  zhanghao->setText(tu);
    if (!tp.isEmpty()){
        mima->setText(tp);
        ui->REpassword->setChecked(true);
        ifRePassword=true;
    }

    // 回车登录
    zhanghao->connect(zhanghao, &QLineEdit::returnPressed, this, &LogIn::on_LogIn1_clicked);
    mima->connect(mima, &QLineEdit::returnPressed, this, &LogIn::on_LogIn1_clicked);
}

LogIn::~LogIn()
{
    delete ui;
}

void LogIn::on_SignUp_clicked()
{
    emit signUpRequested();
}


void LogIn::on_LogIn1_clicked()
{
    QString username=zhanghao->text();
    QString password=mima->text();
    QFile file("save_log");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString password1 = ifRePassword? password : QString();
        stream << username << " " << password1;
        file.close();
    } else {
        qDebug() << "Error: 账密临存失败";
    }
    if (username.isEmpty() || password.isEmpty()){
        API::ToastInfo(this,"请输入用户名和密码！",2000);
        return ;
    }
    if(checkzh(username)){
        if (checkzhmm(username, password)) {
            BasicVariable::IfLog=true;
            BasicVariable::user_name=username;
            API::ToastInfo(this,"登陆成功，欢迎您~",2000);
            close();
            emit loginSuccess();
        }
        else {
            BasicVariable::IfLog=false;
            BasicVariable::user_name="";
            API::ToastInfo(this,"登陆失败：密码错误",2000);
        }
    }
    else{
        API::ToastInfo(this,"登陆失败：该账号尚未注册",2000);
    }
}

bool LogIn::checkzh(const QString& username)
{
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/account.json";
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        file.close();

        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("users") && obj["users"].isArray()) {
                QJsonArray users = obj["users"].toArray();
                for (const auto& userValue : users) {
                    if (userValue.isObject()) {
                        QJsonObject userObj = userValue.toObject();
                        if (userObj.contains("username") && userObj["username"].toString() == QString::number(qHash(username))) {
                            return true; // 用户存在
                        }
                    }
                }
            }
        }
    }

    return false; // 用户不存在
}

bool LogIn::checkzhmm(const QString& username, const QString& password)
{
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/account.json";
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        file.close();

        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("users") && obj["users"].isArray()) {
                QJsonArray users = obj["users"].toArray();
                for (const auto& userValue : users) {
                    if (userValue.isObject()) {
                        QJsonObject userObj = userValue.toObject();
                        if (userObj.contains("username") && userObj["username"].toString() == QString::number(qHash(username))) {
                            if (userObj.contains("password") && userObj["password"].toString() == QString::number(qHash(password))) {
                                return true; // 密码匹配
                            }
                            return false; // 用户存在但密码不匹配
                        }
                    }
                }
            }
        }
    }

    return false; // 用户不存在
}

void LogIn::on_REpassword_clicked(bool checked)
{
    ifRePassword=checked;
}

void LogIn::handleBackRequest(){

}

