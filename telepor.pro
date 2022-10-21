#-------------------------------------------------
#
# Project created by QtCreator 2021-03-05T11:58:16
#
#-------------------------------------------------

QT       += core gui sql network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = telepor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        serverStuff.cpp

HEADERS  += mainwindow.h \
    data.h\
    sms.h\
    serverStuff.h\
    database.h\
    file.h

FORMS    += mainwindow.ui
