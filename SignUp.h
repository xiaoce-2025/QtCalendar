#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include <QLineEdit>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QStandardPaths>

namespace Ui {
class SignUp;
}

class SignUp : public QWidget
{
    Q_OBJECT

public:
    explicit SignUp(QWidget *parent = nullptr);
    ~SignUp();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

signals:
    void BackRequest();

private:
    Ui::SignUp *ui;
    QLineEdit *zhanghao;
    QLineEdit *mima;
    QLineEdit *mima_2;

    // 获取存储文件路径
    QString getFilePath();

    // 读取用户数据
    QJsonArray readUserList();

    // 写入用户数据
    bool writeUserList(const QJsonArray& userList);

    // 检查用户是否已注册
    bool alreadySignUp(const QString& username);

    // 添加新用户
    bool addNewUser(const QString& username, const QString& password);
};

#endif // SIGNUP_H
