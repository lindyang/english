#include "mainwindow.h"
#include <QApplication>
#include <QSslSocket>
#include <QDebug>

void sslTelling() {
    bool bSupp = QSslSocket::supportsSsl();
    QString buildVersion = QSslSocket::sslLibraryBuildVersionString();
    QString version = QSslSocket::sslLibraryVersionString();
    qDebug() << bSupp << buildVersion << version;
}



void checkArgv(int argc, char *argv[]) {
    if (argc != 2) {
        qDebug() << "LD_LIBRARY_PATH=/openssl-1.0.1e:$LD_LIBRARY_PATH ./player <articleId>";
        exit(-1);
    }

    bool isInt;
    QString(argv[1]).toInt(&isInt);
    if (!isInt) {
        qDebug() << "articleId must be int";
        exit(-1);
    }
}


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    MainWindow w("417185362");
    w.show();

    return a.exec();
}
