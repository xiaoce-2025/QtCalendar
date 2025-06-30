#ifndef ACHIEVEMENTSPANEL_H
#define ACHIEVEMENTSPANEL_H

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QWidget>
#include <QScrollArea>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include "achievementwidget.h"

namespace Ui {
class achievementspanel;
}

class achievementspanel : public QWidget
{
    Q_OBJECT


public:
    explicit achievementspanel(QWidget* parent = nullptr);
    void setTotalTasksCompleted(int count);
    void addAchievement(const QString& title, const QString& desc, int target);
    void updateAchievements();
    void init(int totalTasksCompleted);
    ~achievementspanel();

signals:
    void back();

private slots:
    void onBackButtonClicked();  // 添加这行

private:
    Ui::achievementspanel *ui;
    int m_totalTasksCompleted;
    QLabel* m_totalLabel;
    QWidget* m_achievementsContainer;
    QVBoxLayout* m_achievementsLayout;
};

#endif // ACHIEVEMENTSPANEL_H
