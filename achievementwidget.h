#ifndef ACHIEVEMENTWIDGET_H
#define ACHIEVEMENTWIDGET_H

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QWidget>
#include <QScrollArea>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>

namespace Ui {
class achievementwidget;
}

class achievementwidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float progress READ progress WRITE setProgress)


public:
    explicit achievementwidget(const QString& title, const QString& description,int current, int target, bool unlocked, QWidget* parent = nullptr);
    float progress() const;
    void setProgress(float progress);
    void animateUnlock();
    ~achievementwidget();

private:
    Ui::achievementwidget *ui;
    void updateProgressBar();

    QString m_title;
    QString m_description;
    int m_current;
    int m_target;
    bool m_unlocked;
    float m_progress = 0.0f;

    QLabel* m_iconLabel;
    QLabel* m_titleLabel;
    QLabel* m_descriptionLabel;
    QProgressBar* m_progressBar;
    QLabel* m_progressLabel;
    QLabel* m_statusLabel;

private:
    QList<QPropertyAnimation*> m_activeAnimations; // 跟踪动画
};

#endif // ACHIEVEMENTWIDGET_H
