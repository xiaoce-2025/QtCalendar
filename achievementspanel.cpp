#include "achievementspanel.h"
#include <QTimer>

achievementspanel::achievementspanel( QWidget* parent):QWidget(parent){}
void achievementspanel::init(int totalTasksCompleted){
    m_totalTasksCompleted = totalTasksCompleted;

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 在顶部添加返回按钮和标题的布局 (放在mainLayout的开头)
    QHBoxLayout* topBarLayout = new QHBoxLayout();
    topBarLayout->setSpacing(20);

    // 返回按钮
    /*
    QPushButton* backButton = new QPushButton("←", this);
    backButton->setFixedSize(40, 40);
    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #E3F2FD;"
        "   color: 1976D2;"
        "   border: none;"
        "   border-radius: 20px;"
        "   font-size: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #BBDEFB;"
        "}"
        );
    connect(backButton, &QPushButton::clicked, this, &achievementspanel::onBackButtonClicked);
    topBarLayout->addWidget(backButton);
    */

    // 标题
    QLabel* titleLabel = new QLabel("成就系统", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   color: #0D47A1;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "}"
        );
    topBarLayout->addWidget(titleLabel);
    topBarLayout->addStretch(); // 使标题居中

    mainLayout->addLayout(topBarLayout);  // 添加到主布局

    // 总任务完成数
    m_totalLabel = new QLabel(QString("总任务完成数: %1").arg(m_totalTasksCompleted), this);
    m_totalLabel->setStyleSheet(
        "QLabel {"
        "   color: #1976D2;"
        "   font-size: 16px;"
        "}"
        );
    mainLayout->addWidget(m_totalLabel);

    // 成就列表容器
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "   border: none;"
        "   background: transparent;"
        "}"
        "QScrollBar:vertical {"
        "   border: none;"
        "   background: #2D2D2D;"
        "   width: 10px;"
        "   margin: 0px 0px 0px 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #4D4D4D;"
        "   min-height: 20px;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   background: none;"
        "}"
        );

    m_achievementsContainer = new QWidget();
    m_achievementsLayout = new QVBoxLayout(m_achievementsContainer);
    m_achievementsLayout->setSpacing(8);
    m_achievementsLayout->setContentsMargins(5, 5, 5, 5);

    // 添加成就
    addAchievement("初出茅庐", "完成1个任务", 1);
    addAchievement("初窥门径", "完成30个任务", 30);
    addAchievement("登堂入室", "完成328个任务", 328);
    addAchievement("登峰造极", "完成648个任务", 648);

    scrollArea->setWidget(m_achievementsContainer);
    mainLayout->addWidget(scrollArea);
}

void achievementspanel::onBackButtonClicked() {
    emit back();  // 发射返回信号
}

void achievementspanel::addAchievement(const QString& title, const QString& desc, int target) {
    bool unlocked = (m_totalTasksCompleted >= target);
    achievementwidget* achievement = new achievementwidget(
        title,
        desc,
        m_totalTasksCompleted,
        target,
        unlocked,
        this
        );
    m_achievementsLayout->addWidget(achievement);

    // 如果是刚解锁的成就，添加动画效果
    if (unlocked && m_totalTasksCompleted == target) {
        QTimer::singleShot(1000, achievement, [achievement]() {
            if (achievement)  // 检查对象是否存活
                achievement->animateUnlock();
        });
    }
}

void achievementspanel::updateAchievements() {
    // 清除旧成就
    QLayoutItem* child;
    while ((child = m_achievementsLayout->takeAt(0))) {
        delete child->widget();
        delete child;
    }

           // 更新总任务数显示
           m_totalLabel->setText(QString("总任务创建数: %1").arg(m_totalTasksCompleted));

           // 重新添加成就
           addAchievement("初出茅庐", "完成1个任务", 1);
           addAchievement("初窥门径", "完成30个任务", 30);
           addAchievement("登堂入室", "完成328个任务", 328);
           addAchievement("登峰造极", "完成648个任务", 648);
}
void achievementspanel::setTotalTasksCompleted(int count) {
    m_totalTasksCompleted = count;
    updateAchievements();
}

achievementspanel::~achievementspanel()
{
}
