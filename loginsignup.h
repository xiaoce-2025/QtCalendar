#ifndef LOGINSIGNUP_H
#define LOGINSIGNUP_H

#include <QWidget>
#include "LogIn.h"
#include "SignUp.h"

namespace Ui {
class LogInSignUp;
}

class LogInSignUp : public QWidget
{
    Q_OBJECT

public:
    explicit LogInSignUp(QWidget *parent = nullptr);
    ~LogInSignUp();

signals:
    void loginSuccess();

private:
    Ui::LogInSignUp *ui;
};

#endif // LOGINSIGNUP_H
