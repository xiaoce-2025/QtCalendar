#include "MyMessageBox.h"
#include <QLayout>
#include <QStyle>

MyMessageBox::MyMessageBox(QWidget *parent,
                                   const QString &title,
                                   const QString &text,
                                   Icon icon,
                                   StandardButtons buttons)
    : QMessageBox(parent)
{
    setWindowTitle(title);
    setText(text);
    setIcon(icon);
    setStandardButtons(buttons);

    applyStyleSheet();
    setupUI();
}

void MyMessageBox::applyStyleSheet()
{
    QString style = R"(
        QMessageBox {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #f5f7fa, stop:1 #c3cfe2);
            border-radius: 8px;
        }
        QMessageBox QLabel {
            font: 14px "微软雅黑";
            color: #2c3e50;
        }
        QMessageBox QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3498db, stop:1 #2980b9);
            color: white;
            border: none;
            padding: 8px 20px;
            border-radius: 6px;
            font: bold 13px "微软雅黑";
            min-width: 80px;
        }
        QMessageBox QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2980b9, stop:1 #3498db);
        }
        QMessageBox QPushButton:pressed {
            background: #2c3e50;
        }
        QMessageBoxIconLabel {
            min-width: 60px;
            min-height: 60px;
        }
    )";

    setStyleSheet(style);
}

void MyMessageBox::setupUI()
{
    // 按钮布局
    if(QGridLayout* layout = qobject_cast<QGridLayout*>(this->layout())) {
        layout->setHorizontalSpacing(20);
        layout->setVerticalSpacing(15);
        layout->setContentsMargins(20, 20, 20, 20);
    }

    // 图标
    QPixmap iconPixmap = this->iconPixmap();
    if(!iconPixmap.isNull()) {
        setIconPixmap(iconPixmap.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
