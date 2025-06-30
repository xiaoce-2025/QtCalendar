#include "achievementwidget.h"

achievementwidget::achievementwidget(const QString& title, const QString& description,
                                     int current, int target, bool unlocked, QWidget* parent)
    : QWidget(parent), m_title(title), m_description(description),
    m_current(current), m_target(target), m_unlocked(unlocked) {

    this->setFixedHeight(90);
    this->setStyleSheet(
        "AchievementWidget {"
        "   background-color: white;"
        "   border-radius: 10px;"
        "   padding: 15px;"
        "   border: 1px solid #E0E0E0;"  // 添加浅灰色边框
        "}"
        );

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(15);

    // 图标区域
    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(60, 60);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 30px;"
        "   color: #AAAAAA;"
        "   background-color: #3D3D3D;"
        "   border-radius: 30px;"
        "}"
        );
    m_iconLabel->setText("★");
    mainLayout->addWidget(m_iconLabel);

    // 文字区域
    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setSpacing(3);

    m_titleLabel = new QLabel(m_title, this);
    m_titleLabel->setStyleSheet(
        "QLabel {"
        "   color: #1976D2;"
        "   font-weight: bold;"
        "   font-size: 16px;"
        "}"
        );
    textLayout->addWidget(m_titleLabel);

    m_descriptionLabel = new QLabel(m_description, this);
    m_descriptionLabel->setStyleSheet(
        "QLabel {"
        "   color: #2196F3;"
        "   font-size: 12px;"
        "}"
        );
    textLayout->addWidget(m_descriptionLabel);

    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setTextVisible(false);
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "   border: 1px solid #BBDEFB;"
        "   border-radius: 5px;"
        "   background: #E3F2FD;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #2196F3;"
        "   border-radius: 5px;"
        "}"
        );
    textLayout->addWidget(m_progressBar);

    // 进度文本
    m_progressLabel = new QLabel(this);
    m_progressLabel->setStyleSheet(
        "QLabel {"
        "   color: #AAAAAA;"
        "   font-size: 12px;"
        "}"
        );
    textLayout->addWidget(m_progressLabel);

    mainLayout->addLayout(textLayout);

    // 右侧状态指示
    m_statusLabel = new QLabel(this);
    m_statusLabel->setFixedSize(60, 60);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 14px;"
        "   color: #1976D2;"
        "}"
        );
    if (m_unlocked) {
        m_statusLabel->setText("已解锁");
    } else {
        m_statusLabel->setText("未解锁");
    }
    mainLayout->addWidget(m_statusLabel);

    setProgress(0); // 初始化为0，后面动画会更新

    // 添加阴影效果
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 100));
    shadow->setOffset(3, 3);
    this->setGraphicsEffect(shadow);

    // 如果是已解锁的成就，立即显示完成状态
    if (m_unlocked) {
        m_progress = 1.0f;
        updateProgressBar();
        m_iconLabel->setStyleSheet("color: #1976D2;");
        m_titleLabel->setStyleSheet("color: #1565C0; font-weight: bold;");
    }
}
float achievementwidget::progress() const { return m_progress; }

void achievementwidget::setProgress(float progress) {
    m_progress = progress;
    updateProgressBar();
}

void achievementwidget::animateUnlock() {
    m_unlocked = true;
    // 进度条动画
    QPropertyAnimation* progressAnim = new QPropertyAnimation(this, "progress");
    m_activeAnimations.append(progressAnim);
    progressAnim->setDuration(1000);
    progressAnim->setStartValue(0.0f);
    progressAnim->setEndValue(1.0f);
    progressAnim->setEasingCurve(QEasingCurve::OutQuad);
    progressAnim->start();

    // 颜色变化动画
    QPropertyAnimation* colorAnim = new QPropertyAnimation(this);
    m_activeAnimations.append(colorAnim);
    colorAnim->setTargetObject(m_iconLabel);
    colorAnim->setPropertyName("styleSheet");
    colorAnim->setDuration(1000);
    colorAnim->setStartValue("color: #AAAAAA;");
    colorAnim->setEndValue("color: #FFD700;");
    colorAnim->start();

    QPropertyAnimation* titleColorAnim = new QPropertyAnimation(this);
    m_activeAnimations.append(titleColorAnim);
    titleColorAnim->setTargetObject(m_titleLabel);
    titleColorAnim->setPropertyName("styleSheet");
    titleColorAnim->setDuration(1000);
    titleColorAnim->setStartValue("color: #FFFFFF; font-weight: bold;");
    titleColorAnim->setEndValue("color: #FFD700; font-weight: bold;");
    titleColorAnim->start();
}

void achievementwidget::updateProgressBar() {
    float percentage = (m_target > 0) ? (static_cast<float>(m_current) / m_target) : 0.0f;
    percentage = qMin(percentage, 1.0f);

    int visualProgress = static_cast<int>(percentage * 100 * m_progress);
    m_progressBar->setValue(visualProgress);

    if (m_unlocked) {
        m_progressLabel->setText(QString("已完成: %1/%2 (已解锁)").arg(m_target).arg(m_target));
    } else {
        m_progressLabel->setText(QString("已完成: %1/%2").arg(m_current).arg(m_target));
    }
}

achievementwidget::~achievementwidget()
{
    for (QPropertyAnimation* anim : m_activeAnimations) {
        anim->stop();
        anim->deleteLater();
    }
}
