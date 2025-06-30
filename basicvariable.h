#ifndef BASICVARIABLE_H
#define BASICVARIABLE_H
#include <QString>
#include <QSettings>
#include <QDir>

class BasicVariable
{
public:
    BasicVariable();
    static QString user_name;
    static bool IfLog;
    static bool wechatRunning;
    static bool remind15MinBefore;
    static bool remindAtStart;
    static QStringList params;
    static QString startPython;
    static QStringList wechatUserlist;
    static bool closeToExit;
    static bool isDeveloping;
};

#endif // BASICVARIABLE_H
