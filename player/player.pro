#-------------------------------------------------
#
# Project created by QtCreator 2023-06-06T14:27:37
#
#-------------------------------------------------

QT       += core gui widgets multimedia network
#LIBS += -L/home/oem/hello/openssl-1.0.1e -lssl -lcrypto
#INCLUDEPATH += /home/oem/hello/openssl-1.0.1e/include

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = player
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    htmlparser.cpp

HEADERS  += mainwindow.h \
    htmlparser.h

FORMS    += mainwindow.ui
