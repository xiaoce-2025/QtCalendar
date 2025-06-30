#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QApplication>
#include <QPoint>
#include <QScreen>
#include <QPropertyAnimation>

class ToastWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToastWidget(QWidget *parent = nullptr, int displayTime = 2000)
        : QWidget(parent), m_displayTime(displayTime > 0 ? displayTime : 2000)
    {
        // 无边框等属性
        setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_DeleteOnClose);
        setupUI();
        // 一段时间后自动消失
        QTimer::singleShot(m_displayTime, this, &ToastWidget::animateClose);
        // 居中显示
        QTimer::singleShot(0, this, &ToastWidget::adjustPosition);
    }

    void setText(const QString &text)
    {
        label->setText(text);
        adjustSize();
    }

protected:
    void showEvent(QShowEvent *event) override
    {
        QWidget::showEvent(event);
        animateShow();
    }

private slots:
    void animateShow()
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
        animation->setDuration(200);
        animation->setStartValue(0);
        animation->setEndValue(1);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    void animateClose()
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
        animation->setDuration(500);
        animation->setStartValue(1);
        animation->setEndValue(0);
        connect(animation, &QPropertyAnimation::finished, this, &ToastWidget::close);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    void setupUI()
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        label = new QLabel(this);

        label->setStyleSheet(
            "color: white;"
            "background-color: rgba(0, 0, 0, 180);"
            "padding: 8px 12px;"
            "border-radius: 8px;"
            "font: 14px 'Microsoft YaHei';");

        layout->addWidget(label);
        layout->setContentsMargins(20, 10, 20, 10);
    }

    void adjustPosition()
    {
        QWidget *parent = parentWidget();
        if (parent)
        {
            // 居中在父窗口
            // qDebug()<<parent->mapToGlobal(frameGeometry().topLeft())<<parent->frameGeometry().center();
            QPoint des = parent->mapToGlobal(QPoint(0, 0));
            des.setX(des.x() + parent->size().width() / 2);
            des.setY(des.y() + parent->size().height() / 2);
            des -= rect().center();
            move(des);
        }
        else
        {
            // 居中在屏幕
            QScreen *screen = QGuiApplication::primaryScreen();
            QRect screenGeometry = screen->availableGeometry();
            move(
                (screenGeometry.width() - width()) / 2,
                screenGeometry.height() * 0.8 - height() / 2);
        }
    }

    QLabel *label;
    int m_displayTime;
};
