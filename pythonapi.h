#include <QObject>
#include <QProcess>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>

class PythonAPI : public QObject {
    Q_OBJECT
public:
    explicit PythonAPI(QObject *parent = nullptr);
    void startwxautoScript();
    void stopwxautoScript();

signals:
    void wxscheduleReceived(const QJsonObject &event);

private slots:
    void handlewxautoOutput();

private:
    QProcess *m_process;
    QByteArray m_buffer;
};
