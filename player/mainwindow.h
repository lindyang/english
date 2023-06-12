#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <entextedit.h>


class QMediaPlayer;
class HTMLParser;
class DictModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    using DataType = QVector<QMap<QString, QString> >;
    explicit MainWindow(const QString &pageNo_, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_playPushBtn_toggled(bool checked);
    void on_nextPushBtn_clicked();
    void on_prevPushBtn_clicked();

    void on_parseDone(const DataType &data);
    void on_wordClicked(const QVector<QStringList> &dictData);

private slots:
    void on_dictTableView_clicked(const QModelIndex &index);

private:
    bool playAudio(const QString& url);
    void getAudioUrl();


private:
    Ui::MainWindow *ui = nullptr;
    HTMLParser *parser = nullptr;
    QMediaPlayer *player = nullptr;
    const DataType *data = nullptr;
    int idx = -1;
    QString audioUrl;
    QNetworkAccessManager manager;
    QString articleIdStr;

    DictModel *dictModel;
    QMediaPlayer *pronPlayer = nullptr;
};

#endif // MAINWINDOW_H
