#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>


class QMediaPlayer;
class HTMLParser;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    using DataType = QVector<QMap<QString, QString>* >;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_playPushBtn_toggled(bool checked);
    void on_nextPushBtn_clicked();
    void on_prevPushBtn_clicked();

    void on_parseDone(DataType *data);

private:
    bool playAudio(const QString& url);
    void uploadAndGetUrl();


private:
    Ui::MainWindow *ui = nullptr;
    HTMLParser *parser = nullptr;
    QMediaPlayer *player = nullptr;
    DataType *data = nullptr;
    int idx = -1;
    QString audioUrl;
    QNetworkAccessManager manager;
};

#endif // MAINWINDOW_H
