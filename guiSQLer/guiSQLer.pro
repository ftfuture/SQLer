#-------------------------------------------------
#
# Project created by QtCreator 2017-04-13T21:25:45
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = guiSQLer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp\
        mainwindow.cpp \
    dbtree.cpp \
    dbdialog.cpp \
    sqlersetting.cpp

HEADERS  += mainwindow.h \
    dbtree.h \
    dbdialog.h \
    sqlersetting.h

FORMS    += mainwindow.ui \
    dbdialog.ui


unix|win32: LIBS += -L$$OUT_PWD/../DBManager/ -lDBManager

INCLUDEPATH += $$PWD/../DBManager
DEPENDPATH += $$PWD/../DBManager

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../DBManager/DBManager.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../DBManager/libDBManager.a
