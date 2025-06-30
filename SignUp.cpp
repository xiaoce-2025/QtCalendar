#include "SignUp.h"
#include "ui_SignUp.h"
#include <QMessageBox>
#include <QtCore/qdir.h>
#include <QtCore/qfileinfo.h>
#include <mymessagebox.h>
#include "api.h"
#include <QDebug>
#include <QHash>

SignUp::SignUp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SignUp)
{
    ui->setupUi(this);
    zhanghao = ui->zhanghao;
    mima = ui->mima;
    mima_2=ui->mima_2;
}

SignUp::~SignUp()
{
    delete ui;
}

QString SignUp::getFilePath()
{
    // 获取用户主目录
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    return homePath + "/account.json";
}

QJsonArray SignUp::readUserList()
{
    QString filePath = getFilePath();
    QFile file(filePath);

    // 如果文件不存在，返回空数组
    if (!file.exists()) {
        return QJsonArray();
    }

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        file.close();

        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("users") && obj["users"].isArray()) {
                return obj["users"].toArray();
            }
        }
    }

    return QJsonArray();
}

bool SignUp::writeUserList(const QJsonArray& userList)
{
    QString filePath = getFilePath();
    QFile file(filePath);

    // 创建父目录（如果不存在）
    QFileInfo fileInfo(filePath);
    QDir().mkpath(fileInfo.absolutePath());

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QJsonObject root;
        root["users"] = userList;

        QJsonDocument doc(root);
        file.write(doc.toJson());
        file.close();
        return true;
    }

    qDebug() << "Error: Could not write to file:" << filePath;
    return false;
}

bool SignUp::alreadySignUp(const QString& username)
{
    QJsonArray userList = readUserList();

    // 检查每个用户对象是否包含该用户名
    for (const auto& userValue : userList) {
        if (userValue.isObject()) {
            QJsonObject userObj = userValue.toObject();
            if (userObj.contains("username") && userObj["username"].toString() == QString::number(qHash(username))) {
                return false; // 用户名已存在
            }
        }
    }

    return true; // 用户名不存在，可以注册
}

bool SignUp::addNewUser(const QString& username, const QString& password)
{
    QJsonArray userList = readUserList();

    // 创建新用户对象
    QJsonObject newUser;
    newUser["username"] = QString::number(qHash(username));
    newUser["password"] = QString::number(qHash(password));

    // 添加到用户列表
    userList.append(newUser);

    // 写回文件
    return writeUserList(userList);
}

void SignUp::on_pushButton_clicked()
{
    QString username=zhanghao->text();
    QString password=mima->text();
    QString password_2=mima_2->text();
    if (username.isEmpty() || password.isEmpty() || password_2.isEmpty()){
        API::ToastInfo(this,"请输入用户名和密码！",2000);
        return ;
    }

    if(!alreadySignUp(username)){
        API::ToastInfo(this,"注册失败：该用户名已被注册",2000);
    }
    else if(password!=password_2){
        API::ToastInfo(this,"注册失败：两次输入的密码不一致",2000);
    }
    else{
        // 添加新用户到文件
        if (addNewUser(username, password)) {
            API::ToastInfo(this,"注册成功",2000);
            emit BackRequest();
            close();
        } else {
            API::ToastInfo(this,"注册失败：无法保存用户信息",2000);
        }
    }

    // 回车注册
    zhanghao->connect(zhanghao, &QLineEdit::returnPressed, this, &SignUp::on_pushButton_clicked);
    mima->connect(mima, &QLineEdit::returnPressed, this, &SignUp::on_pushButton_clicked);
    mima_2->connect(mima_2, &QLineEdit::returnPressed, this, &SignUp::on_pushButton_clicked);
}


void SignUp::on_pushButton_2_clicked()
{
    emit BackRequest();
    close();
}

