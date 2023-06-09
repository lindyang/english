#-------------------------------------------------
#
# Project created by QtCreator 2023-06-06T14:27:37
#
#-------------------------------------------------

QT       += core gui widgets multimedia network

unix {
        LIBS += -L//openssl-1.0.1e -lssl -lcrypto
        INCLUDEPATH += /openssl-1.0.1e/include
	QMAKE_LFLAGS += -no-pie
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = player
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    htmlparser.cpp \
    entextedit.cpp

HEADERS  += mainwindow.h \
    htmlparser.h \
    entextedit.h \
    requests.h

FORMS    += mainwindow.ui
