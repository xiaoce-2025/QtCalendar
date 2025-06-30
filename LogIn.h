#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QLineEdit>
#include <QMessageBox>

namespace Ui {
class LogIn;
}

class LogIn : public QWidget
{
    Q_OBJECT

public:
    explicit LogIn(QWidget *parent = nullptr);
    ~LogIn();

private slots:
    void on_SignUp_clicked();

    void on_LogIn1_clicked();

    void on_REpassword_clicked(bool checked);

    void handleBackRequest();

signals:
    void loginSuccess();
    void signUpRequested();

private:
    Ui::LogIn *ui;
    QLineEdit *zhanghao;
    QLineEdit *mima;
    bool checkzhmm(const QString& username, const QString& password);
    bool checkzh(const QString& username);
    bool ifRePassword=false;
};

#endif // LOGIN_H
