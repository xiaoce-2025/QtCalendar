#include "sourceapi.h"
#include <QFile>
#include <QJsonDocument>
#include <QDir>
#include "basicvariable.h"
#include "ToastWidget.h"


SourceAPI::SourceAPI() {}


bool SourceAPI::CheckEvent(QJsonObject MyEvent){
    if (MyEvent.contains("id") && MyEvent["id"].isString() &&
        MyEvent.contains("title") && MyEvent["title"].isString()){
        if (MyEvent.contains("start") && MyEvent["start"].isString() && MyEvent["start"].toString()!="NULL"){
            return true;
        }
        else if (MyEvent.contains("end") && MyEvent["end"].isString() && MyEvent["end"].toString()!="NULL"){
            return true;
        }
    }
    return false;
}



// 创建事件，true表示成功
bool SourceAPI::CreateEvent(QJsonObject MyEvent,QString rePath){
    if (!SourceAPI::CheckEvent(MyEvent)){
        qDebug()<<"ERROR#1011 未通过事件完整性校验";
        qDebug()<<MyEvent;
        return false;
    }
    MyEvent["editTime"] = QDateTime::currentDateTime().toString();
    QString year_month="2020-09";
    if ((MyEvent.contains("start") && MyEvent["start"].isString() && MyEvent["start"].toString()!="NULL")){
        QString input = MyEvent["start"].toString();
        QDateTime dt = QDateTime::fromString(input, Qt::ISODate);
        year_month = dt.toString("yyyy-MM");
    }
    else if (MyEvent.contains("end") && MyEvent["end"].isString() && MyEvent["end"].toString()!="NULL"){
        QString input = MyEvent["end"].toString();
        QDateTime dt = QDateTime::fromString(input, Qt::ISODate);
        year_month = dt.toString("yyyy-MM");
    }
    else{
        qDebug()<<"ERROR#1010 未能成功解析事件年月,采用默认值2020-09";
    }
    // 获取相对路径
    QString relativePath = "data/users/";
    relativePath+=BasicVariable::user_name;
    relativePath+=rePath;
    QString absolutePath = QDir::currentPath() + "/" + relativePath;
    QDir dir(absolutePath);
    if (!dir.exists()) {
        dir.mkpath("."); // 递归创建目录
    }

    //以下为打开index
    QFile jsonFile(absolutePath+"_index.json");
    // 若月份索引不存在则创建
    if (!QFileInfo::exists(absolutePath+"_index.json")){
        QJsonObject temp_001;
        QJsonArray array;
        if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "ERROR#1003 月份索引创建失败:" << jsonFile.errorString();
            return false;
        }

        // 写入文件
        jsonFile.write(QJsonDocument(temp_001).toJson(QJsonDocument::Indented));
        jsonFile.close();
    }
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ERROR#1001 打开月份索引失败:" << jsonFile.errorString();
        return false;
    }
    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();
    // 解析 JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "ERROR#1002 月份索引json解析失败:" << parseError.errorString();
        return false;
    }
    // 检查目录
    if (doc.isObject()) {
        QJsonObject jsonObj = doc.object();
        if (!jsonObj.contains(year_month)){
            QJsonArray array;
            jsonObj[year_month]=array;
            if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qDebug() << "ERROR#1004 月份索引覆写失败:" << jsonFile.errorString();
                return false;
            }
            jsonFile.write(QJsonDocument(jsonObj).toJson(QJsonDocument::Indented));
            jsonFile.close();

            QFile jsonFile(absolutePath+year_month+".json");
            QString nestedJson = R"({
                "schedules": [],
                "statistics": {
                    "total_schedules": 0,
                    "categories_distribution": {}
                    }
                })";
            QJsonDocument nestedDoc = QJsonDocument::fromJson(nestedJson.toUtf8());
            QJsonObject r = nestedDoc.object();
            r["year-month"]=year_month;

            if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qDebug() << "ERROR#1004 月份索引覆写失败:" << jsonFile.errorString();
                return false;
            }
            jsonFile.write(QJsonDocument(r).toJson(QJsonDocument::Indented));
            jsonFile.close();
        }
    }

    jsonFile.setFileName(absolutePath+year_month+".json");
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ERROR#1005 打开月份事件失败:" << jsonFile.errorString();
        return false;
    }
    jsonData = jsonFile.readAll();
    jsonFile.close();
    // 解析JSON
    doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "ERROR#1006 月份事件解析失败:" << parseError.errorString();
        return false;
    }
    // 添加事件
    if (doc.isObject()){
        QJsonObject jsonObj = doc.object();

        // 处理schedules数组
        if (jsonObj.contains("schedules") && jsonObj["schedules"].isArray()) {
            QJsonArray schedules = jsonObj["schedules"].toArray();
            schedules.append(MyEvent);
            jsonObj["schedules"] = schedules;
        }
        else{
            qDebug() << "ERROR#1007 json格式校验未通过";
            return false;
        }

        // 处理statistics统计
        if (jsonObj.contains("statistics") && jsonObj["statistics"].isObject()) {
            QJsonObject stats = jsonObj["statistics"].toObject();
            stats["total_schedules"] = stats.value("total_schedules").toInt(0) + 1;
            jsonObj["statistics"] = stats;
        }
        else{
            qDebug() << "ERROR#1008 json格式校验未通过";
            return false;
        }

        // 此处还可以添加分类索引，但这里先省略了

        // 写入文件
        if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "ERROR#1009 月份事件覆写失败:" << jsonFile.errorString();
            return false;
        }
        jsonFile.write(QJsonDocument(jsonObj).toJson(QJsonDocument::Indented));
        jsonFile.close();

        // 写入索引
        jsonFile.setFileName(absolutePath+"_index.json");
        if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "ERROR#1012 打开月份索引失败:" << jsonFile.errorString();
            return false;
        }
        jsonData = jsonFile.readAll();
        jsonFile.close();
        // 解析 JSON
        doc = QJsonDocument::fromJson(jsonData, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "ERROR#1013 月份索引解析失败:" << parseError.errorString();
            return false;
        }
        // 添加事件索引
        if (doc.isObject()){
            QJsonObject jsonObj = doc.object();
            QJsonArray ind = jsonObj[year_month].toArray();
            ind.append(MyEvent["id"].toString());
            jsonObj[year_month] = ind;
            if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qDebug() << "ERROR#1014 月份索引覆写失败:" << jsonFile.errorString();
                return false;
            }
            jsonFile.write(QJsonDocument(jsonObj).toJson(QJsonDocument::Indented));
            jsonFile.close();
        }
        qDebug()<<"事件写入完成！";
        return true;
    }
    return false;
}


// 检查事件是否存在，返回值不为空串为存在
QString SourceAPI::SearchEvent(QString id,QString rePath){
    QString relativePath = "data/users/";
    relativePath+=BasicVariable::user_name;
    relativePath+=rePath;
    QString absolutePath = QDir::currentPath() + "/" + relativePath;
    QDir dir(absolutePath);
    if (!dir.exists()) {
        dir.mkpath("."); // 递归创建目录
    }
    absolutePath+="_index.json";
    QFile file(absolutePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "ERROR#1015 无法打开文件:" << file.errorString();
        return "";
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (jsonDoc.isNull()) {
        qWarning() << "ERROR#1016 JSON解析错误:" << parseError.errorString();
        return "";
    }

    if (!jsonDoc.isObject()) {
        qWarning() << "ERROR#1017 根元素不是JSON对象";
        return "";
    }

    QJsonObject rootObj = jsonDoc.object();
    for (auto it = rootObj.begin(); it != rootObj.end(); ++it) {
        if (it->isArray()) {
            QJsonArray array = it->toArray();
            for (const QJsonValue &value : array) {
                if (value.isString() && value.toString() == id) {
                    return QString(it.key());
                }
            }
        }
    }

    return "";
}


// 修改事件,true表示成功
bool SourceAPI::UpdateEvent(QString id,QJsonObject NewEvent,QString rePath){
    if (SourceAPI::RetrieveEvent(id,rePath).isEmpty()){
        qDebug()<<"ERROR#1027 事件不存在";
        return false;
    }
    if (SourceAPI::DeleteEvent(id,rePath)){
        if (SourceAPI::CreateEvent(NewEvent,rePath)){
            return true;
        }
    }
    return false;
}


// 查询事件 事件不存在返回空对象
QJsonObject SourceAPI::RetrieveEvent(QString id,QString rePath){
    QString year_month=SourceAPI::SearchEvent(id,rePath);
    if (year_month.isEmpty()){
        return QJsonObject();
    }

    QString relativePath = "data/users/";
    relativePath+=BasicVariable::user_name;
    relativePath+=rePath;
    QString absolutePath = QDir::currentPath() + "/" + relativePath;
    QDir dir(absolutePath);
    if (!dir.exists()) {
        dir.mkpath("."); // 递归创建目录
    }
    QFile jsonFile(absolutePath+year_month+".json");
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ERROR#1018 打开月份事件失败:" << jsonFile.errorString();
        return QJsonObject();
    }
    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();
    // 解析 JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "ERROR#1019 月份事件解析失败:" << parseError.errorString();
        return QJsonObject();
    }

    // 获取根对象
    QJsonObject root = doc.object();
    if (!root.contains("schedules")) {
        qDebug() << "ERROR#1020 json文件校验错误";
        return QJsonObject();
    }

    // 获取 schedules 数组
    QJsonArray schedules = root["schedules"].toArray();

    // 遍历查找目标 ID
    for (const QJsonValue &value : schedules) {
        QJsonObject schedule = value.toObject();
        if (schedule["id"].toString() == id) {
            return schedule; // 返回匹配的 QJsonObject
        }
    }

    qDebug() << "ERROR#1021 未找到目标事件:" << id;
    return QJsonObject(); // 未找到则返回空对象
}


// 查询某一月份事件 月份不存在返回空对象
QJsonArray SourceAPI::RetrieveMonthEvent(QString year_month,QString rePath) {
    QString relativePath = "data/users/";
    relativePath += BasicVariable::user_name;
    relativePath += rePath;
    QString absolutePath = QDir::currentPath() + "/" + relativePath;

    QDir dir(absolutePath);
    if (!dir.exists()) {
        dir.mkpath("."); // 递归创建目录
    }

    QFile jsonFile(absolutePath + year_month + ".json");
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ERROR#1029 打开月份事件失败:" << jsonFile.errorString();
        return QJsonArray();
    }

    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "ERROR#1027 月份事件解析失败:" << parseError.errorString();
        return QJsonArray();
    }

    QJsonObject root = doc.object();
    if (!root.contains("schedules")) {
        qDebug() << "ERROR#1028 json文件校验错误";
        return QJsonArray();
    }

    return root["schedules"].toArray(); // 直接返回整个事件数组
}


// 查询所有月份 返回列表
QStringList SourceAPI::RetrieveAllEventMonth(QString rePath){
    QString relativePath = "data/users/";
    relativePath += BasicVariable::user_name;
    relativePath += rePath;
    QString absolutePath = QDir::currentPath() + "/" + relativePath;

    QDir dir(absolutePath);
    if (!dir.exists()) {
        dir.mkpath("."); // 递归创建目录
    }

    QFile indexFile(absolutePath + "_index.json");
    if (!indexFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ERROR#1030 打开月份事件失败:" << indexFile.errorString();
        return QStringList();
    }


    // 读取并解析JSON
    QByteArray jsonData = indexFile.readAll();
    indexFile.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "ERROR#1031 JSON解析失败:" << parseError.errorString();
        return QStringList();
    }

    // 验证根对象结构
    if (!doc.isObject()) {
        qDebug() << "ERROR#1032 索引文件格式错误";return QStringList();
    }

    // 提取所有键值
    return doc.object().keys();
}


// 删除事件，true为删除成功
bool SourceAPI::DeleteEvent(QString id,QString rePath){
    QString year_month=SourceAPI::SearchEvent(id,rePath);
    if (year_month.isEmpty()){
        return false;
    }

    QString relativePath = "data/users/";
    relativePath+=BasicVariable::user_name;
    relativePath+=rePath;
    QString absolutePath = QDir::currentPath() + "/" + relativePath;
    QDir dir(absolutePath);
    if (!dir.exists()) {
        dir.mkpath("."); // 递归创建目录
    }
    QFile jsonFile(absolutePath+year_month+".json");
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ERROR#1022 打开月份事件失败:" << jsonFile.errorString();
        return false;
    }
    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();
    // 解析 JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "ERROR#1023 月份事件解析失败:" << parseError.errorString();
        return false;
    }

    // 获取根对象
    QJsonObject root = doc.object();
    if (!root.contains("schedules")) {
        qDebug() << "ERROR#1024 json文件校验错误";
        return false;
    }

    // 获取 schedules 数组
    QJsonArray schedules = root["schedules"].toArray();

    bool found = false;

    // 遍历查找目标 ID
    for (int i = 0; i < schedules.size(); ++i) {
        QJsonObject schedule = schedules[i].toObject();
        if (schedule["id"].toString() == id) {
            schedules.removeAt(i); // 删除匹配项
            found = true;
            break;
        }
    }

    if (!found) {
        qDebug() << "ERROR#1025 未找到目标事件:" << id;
        return false;
    }

    // 更新 JSON 结构
    root["schedules"] = schedules;

    //处理统计部分
    if (root.contains("statistics") && root["statistics"].isObject()) {
        QJsonObject stats = root["statistics"].toObject();
        stats["total_schedules"] = stats.value("total_schedules").toInt(0) - 1;
        root["statistics"] = stats;
    }
    else{
        qDebug() << "ERROR#1033 json格式校验未通过";
        return false;
    }


    doc.setObject(root);

    // 写入删除后的文件
    if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "ERROR#1026 月份索引覆写失败:" << jsonFile.errorString();
        return false;
    }
    jsonFile.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));
    jsonFile.close();
    return true;
}
