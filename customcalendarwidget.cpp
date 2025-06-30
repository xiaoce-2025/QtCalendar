// CustomCalendarWidget.cpp
#include "CustomCalendarWidget.h"
#include <QPainter>
#include <QHeaderView>
#include <QStyleOptionViewItem>
#include <QTableView>
#include <QPainterPath>

CustomCalendarWidget::CustomCalendarWidget(QWidget *parent)
    : QCalendarWidget(parent), scheduleMap(nullptr)
{
    // 隐藏周数列
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
}


void CustomCalendarWidget::setScheduleMap(const QMap<QDate, QStringList> *map) {
    scheduleMap = map;
}

void CustomCalendarWidget::paintCell(QPainter *painter, const QRect &rect, QDate date) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);//抗锯齿

    // 状态判断
    bool isSelected = date == selectedDate();
    bool isToday = date == QDate::currentDate();
    bool isOtherMonth = date.month() != monthShown();
    bool hasEvent = scheduleMap && scheduleMap->contains(date) && !scheduleMap->value(date).isEmpty();

    // 圆角
    QPainterPath path;
    int radius = 5;
    path.addRoundedRect(rect, radius, radius);

    // 选中样式
    if (isSelected) {
        painter->fillPath(path, QColor(245, 245, 245)); // 浅灰色背景
    }

    // 本月日期样式
    if (!isOtherMonth) {
        // 当日样式
        if (isToday) {
            painter->fillPath(path, QColor(173, 216, 230, 80));
        }
    }

    // 非本月样式
    if (isOtherMonth) {
        painter->fillPath(path, QColor(225, 225, 225)); // 置灰背景
    }

    // 文字颜色
    QColor textColor = isOtherMonth ? QColor(180, 180, 180) :
                           (isSelected ? Qt::black : palette().color(QPalette::Text));
    painter->setPen(textColor);

    // 绘制日期数字
    painter->drawText(rect.adjusted(5, 3, -5, -3),
                      Qt::AlignTop | Qt::AlignLeft,
                      QString::number(date.day()));

    // 事件列表（若当日有事件）
    if (!isOtherMonth && hasEvent) {
        const QStringList &events = scheduleMap->value(date);
        int showEvents = qMin(3, events.size());
        QRect eventRect = rect.adjusted(5, 25, -5, -5);

        painter->setFont(QFont(painter->font().family(), 9));
        painter->setPen(QColor(80, 80, 80));

        for (int i = 0; i < showEvents; ++i) {
            QString text = painter->fontMetrics().elidedText(events[i], Qt::ElideRight, eventRect.width());
            painter->drawText(eventRect, Qt::TextSingleLine | Qt::AlignTop, text);
            eventRect.adjust(0, painter->fontMetrics().height() + 2, 0, 0);
        }
    }

    painter->restore();
}
