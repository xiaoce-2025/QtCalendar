#include "basicvariable.h"
#include <QStandardPaths>
#include <QDir>
#include <QApplication>

BasicVariable::BasicVariable() {

}

QString BasicVariable::user_name="";
bool BasicVariable::IfLog=false;
bool BasicVariable::wechatRunning=false;
bool BasicVariable::remind15MinBefore=false;
bool BasicVariable::remindAtStart=false;
QStringList BasicVariable::params;
QString BasicVariable::startPython;
QStringList BasicVariable::wechatUserlist;
bool BasicVariable::closeToExit = true;
bool BasicVariable::isDeveloping = true;


