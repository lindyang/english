#-------------------------------------------------
#
# Project created by QtCreator 2023-06-06T14:27:37
#
#-------------------------------------------------

QT       += core gui widgets multimedia network

unix {
        LIBS += -L/openssl-1.0.1e -lssl -lcrypto
        INCLUDEPATH += /openssl-1.0.1e/include
	QMAKE_LFLAGS += -no-pie
}

win32 {
        INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/ucrt"
        LIBS += -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/ucrt/x86"
        LIBS += -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x86"
        # rc.exe, rc.dll -> C:/Qt/Qt5.6.3/5.6.3/msvc2015\bin
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = player
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    htmlparser.cpp \
    entextedit.cpp \
    dictmodel.cpp

HEADERS  += mainwindow.h \
    htmlparser.h \
    entextedit.h \
    requests.h \
    dictmodel.h

FORMS    += mainwindow.ui
