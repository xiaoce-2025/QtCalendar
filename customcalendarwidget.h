#pragma once
#include <QCalendarWidget>
#include <QMap>

class CustomCalendarWidget : public QCalendarWidget {
    Q_OBJECT
public:
    explicit CustomCalendarWidget(QWidget *parent = nullptr);
    void setScheduleMap(const QMap<QDate, QStringList> *map);
    void refreshCells() {updateCells();}

protected:
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;

private:
    const QMap<QDate, QStringList> *scheduleMap;
};
