#ifndef NEWWXEVENTSHOW_H
#define NEWWXEVENTSHOW_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class NewWXEventShow;
}

class NewWXEventShow : public QWidget
{
    Q_OBJECT

public:
    explicit NewWXEventShow(QWidget *parent = nullptr);
    ~NewWXEventShow();

private:
    Ui::NewWXEventShow *ui;

public:
    void addMessage(const QJsonObject &msg);

private slots:
    void processSelectedMessages();
    void selectAll();
    void deselectAll();

signals:
    void backRequested();

private:
    QListWidget *listWidget;
    QPushButton *backButton;
    QPushButton *selectAllButton;
    QPushButton *deselectAllButton;
    QList<QCheckBox*> checkBoxes;
    QJsonArray allevent;

};

#endif // NEWWXEVENTSHOW_H
