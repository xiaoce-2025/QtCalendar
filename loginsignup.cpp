#include "loginsignup.h"
#include "ui_loginsignup.h"
#include <QStackedWidget>
#include <QVBoxLayout>

LogInSignUp::LogInSignUp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogInSignUp)
{
    ui->setupUi(this);

    QStackedWidget *stackedWidget = new QStackedWidget;
    stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    LogIn *login = new LogIn;
    SignUp *signup = new SignUp;

    stackedWidget->addWidget(login);
    stackedWidget->addWidget(signup);
    stackedWidget->setCurrentWidget(login);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 连接登录界面的注册信号
    connect(login, &LogIn::signUpRequested, this, [=]() {
        stackedWidget->setCurrentWidget(signup);
    });

    // 连接注册界面的返回信号
    connect(signup, &SignUp::BackRequest, this, [=]() {
        stackedWidget->setCurrentWidget(login);
    });

    // 连接登录界面的登录成功信号
    connect(login, &LogIn::loginSuccess, this, [=]() {
        emit loginSuccess();
        close();
    });
}


LogInSignUp::~LogInSignUp()
{
    delete ui;
}
