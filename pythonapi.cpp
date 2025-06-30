#include "pythonapi.h"
#include "basicvariable.h"
#include <QDir>
#include <QCoreApplication>

PythonAPI::PythonAPI(QObject *parent) : QObject(parent) {
    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &PythonAPI::handlewxautoOutput);
}

void PythonAPI::startwxautoScript() {

    qDebug()<<"尝试创建pyhon微信监听进程";

    // 如果进程已在运行，先终止
    if (m_process->state() != QProcess::NotRunning) {
        qDebug() << "检测到已有进程运行，先终止旧进程";
        stopwxautoScript();
    }
    QString command;
    if (BasicVariable::isDeveloping){
        // python启动参数
        QString PythonQiDong = BasicVariable::startPython;
        PythonQiDong.replace("\n"," && ");
        // python脚本位置，此处构建时改成相对路径，现在没法弄，如使用请修改成自己电脑的路径
        QString scriptPath = "E:/Ccode/chengshe_bighomework/calendar/test2/py/wxevent.py";

        // 启动进程
        command = QString("%1 %2").arg(PythonQiDong).arg(scriptPath);
        // 添加参数
        for (const QString &param : BasicVariable::params) {
            command += " " + param;
        }
        qDebug()<<"[微信监听]python启动命令:"<<command;
    }
    else{
        command = "E:/Ccode/chengshe_bighomework/calendar/test2/py/wxevent.exe";
        for (const QString &param : BasicVariable::params) {
            command += " " + param;
        }
        qDebug()<<"[微信监听]监听进程启动命令:"<<command;
    }
    m_process->start("cmd.exe", {"/C", command});

    // 错误处理
    connect(m_process, &QProcess::errorOccurred, [](QProcess::ProcessError error) {
        qWarning() << "Process error:" << error;
    });

    qDebug()<<"微信监听进程创建成功";
}


// 终止函数
void PythonAPI::stopwxautoScript() {
    if (m_process->state() == QProcess::Running) {
        qDebug() << "终止微信监听";

        // 先尝试正常终止
        m_process->terminate();

        // 等待最多3秒让进程正常退出
        if (!m_process->waitForFinished(3000)) {
            qDebug() << "终止失败，强制终止微信监听";
            m_process->kill();  // 强制终止
            m_process->waitForFinished(); // 等待进程完全退出
        }

        qDebug() << "微信监听已终止";
        m_buffer.clear(); // 清空缓冲区
    }
}


void PythonAPI::handlewxautoOutput() {
    QByteArray data = m_process->readAllStandardOutput();
    m_buffer += data;
    qDebug().noquote() << "[PyhonProcess]" << data;
    // 解析相关输出
    while (true) {
        int startPos = m_buffer.indexOf("QT_DATA_START:");
        if (startPos == -1) break;

        int endPos = m_buffer.indexOf("QT_DATA_END", startPos);
        if (endPos == -1) break;
        //qDebug()<<startPos<<" "<<endPos;
        QByteArray jsonData = m_buffer.mid(startPos + 14, endPos - startPos - 14);
        m_buffer = m_buffer.mid(endPos + 1);
        //qDebug()<<"截取后:"<<jsonData;

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
        //qDebug()<<"转义检测："<<doc;
        if (doc.isNull()){
            qDebug() << "解析错误：" << error.errorString() << "at offset" << error.offset;
        }
        if (error.error == QJsonParseError::NoError && doc.isObject()) {
            emit wxscheduleReceived(doc.object());
        }
    }
}
