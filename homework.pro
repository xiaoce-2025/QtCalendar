QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LogIn.cpp \
    SignUp.cpp \
    SingleEventShow.cpp \
    achievementspanel.cpp \
    achievementwidget.cpp \
    api.cpp \
    basicvariable.cpp \
    calendarwidget.cpp \
    customcalendarwidget.cpp \
    eventwidget.cpp \
    history.cpp \
    homepage.cpp \
    inexport.cpp \
    listviewwidget.cpp \
    loginsignup.cpp \
    main.cpp \
    mainwindow.cpp \
    mymessagebox.cpp \
    newwxeventshow.cpp \
    pythonapi.cpp \
    remindermanager.cpp \
    settings.cpp \
    showconflicts.cpp \
    sourceapi.cpp

HEADERS += \
    LogIn.h \
    SignUp.h \
    SingleEventShow.h \
    ToastWidget.h \
    achievementspanel.h \
    achievementwidget.h \
    api.h \
    basicvariable.h \
    calendarwidget.h \
    customcalendarwidget.h \
    eventwidget.h \
    history.h \
    homepage.h \
    inexport.h \
    listviewwidget.h \
    loginsignup.h \
    mainwindow.h \
    mymessagebox.h \
    newwxeventshow.h \
    pythonapi.h \
    remindermanager.h \
    settings.h \
    showconflicts.h \
    sourceapi.h

FORMS += \
    LogIn.ui \
    SignUp.ui \
    calendarwidget.ui \
    eventwidget.ui \
    history.ui \
    inexport.ui \
    listviewwidget.ui \
    loginsignup.ui \
    mainwindow.ui \
    newwxeventshow.ui \
    settings.ui \
    showconflicts.ui

TRANSLATIONS += \
    homework_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    pictures/pictures.qrc

QT += network
