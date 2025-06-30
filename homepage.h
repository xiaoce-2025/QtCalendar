#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QDate>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include <QGroupBox>
#include <QScrollArea>
#include <QTimer>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);

private slots:
    void updateDateTime();

private:
    void createInfoPanel();
    QPushButton* createDayButton(int day, bool currentMonth = true);

    QHBoxLayout *mainLayout;
    QLabel *labelTime;
    QLabel *labelDate;
    QLabel *labelDay;
    QDateEdit *dateEdit;
    QLabel *weatherIcon;
    QLabel *weatherTemp;
    QLabel *weatherCity;
    QLabel *weatherDescription;

private slots:
    void handleWeatherResponse(QNetworkReply *reply);

private:
    void fetchWeatherData(const QString &cityCode);
    void updateWeatherUI(const QString &city, const QString &type, const QString &temp, const QString &desc);
    QString determineWeatherIcon(const QString &type);

    // 网络管理器
    QNetworkAccessManager *netManager;
};

#endif // HOMEPAGE_H
