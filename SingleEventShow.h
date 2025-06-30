#ifndef SINGLEEVENTSHOW_H
#define SINGLEEVENTSHOW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDesktopServices>
#include <QMenu>
#include "api.h"
#include <QContextMenuEvent>
#include <QPushButton>

class SingleEventShowWidget : public QWidget {
    Q_OBJECT
public:
    explicit SingleEventShowWidget(const QJsonObject &event, QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void setupUI();
    void addTimeField(QHBoxLayout *layout, const QString &label, const QString &value);
    QString getCategoryStyle();
    QString getAttachmentText(const QJsonObject &attachment);
    QJsonObject m_event;
    QJsonObject m_selectedAttachment;

public:
    QPushButton *deleteBtn;
    QPushButton *editBtn;
    QPushButton *finishBtn;


signals:
    void deleteRequested(const QString &eventId);
    void editRequested(const QJsonObject &event);
    void finishRequested(const QJsonObject &event);

private:
    QString m_eventId;

};


#endif // SINGLEEVENTSHOW_H
