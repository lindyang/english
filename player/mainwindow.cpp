#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "htmlparser.h"
#include "requests.h"
#include "dictmodel.h"

#include <QDebug>
#include <QMediaPlayer>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkReply>



MainWindow::MainWindow(const QString &articleIdStr_, QWidget *parent)
    :articleIdStr(articleIdStr_), QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager.setParent(this);
    ui->playPushBtn->setEnabled(true);

    dictModel = new DictModel(this);
    ui->dictTableView->setModel(dictModel);
    ui->dictTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);  // ResizeToContents
    ui->dictTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->dictTableView->horizontalHeader()->setStretchLastSection(true);
    ui->dictTableView->setAlternatingRowColors(true);
    ui->dictTableView->verticalHeader()->hide();

    pronPlayer = new QMediaPlayer(this);

    ui->enTextEdit->setFont(QFont("Times", 16, QFont::Bold));
    connect(ui->enTextEdit, SIGNAL(lookUpDone(const QVector<QStringList>&)), SLOT(on_wordClicked(const QVector<QStringList>&)));

    parser = new HTMLParser(articleIdStr, this);
    connect(parser, SIGNAL(parseDone(const DataType&)), this, SLOT(on_parseDone(const DataType&)));

    player = new QMediaPlayer(this);

    connect(player, static_cast<void(QMediaPlayer::*)(QMediaPlayer::State)>(&QMediaPlayer::stateChanged), this,
        [this](QMediaPlayer::State state) {
            if (state == QMediaPlayer::State::StoppedState) {
                ui->playPushBtn->setText("i>");
                ui->playPushBtn->setChecked(false);
            }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete parser;
    delete player;
    delete pronPlayer;
    delete dictModel;
}

void MainWindow::getAudioUrl() {
    auto sentence = this->data->at(this->idx).value("sentence");
    auto content = QString(sentence).replace(
                QString::fromLocal8Bit("“"), "\"").replace(
                QString::fromLocal8Bit("”"), "\"").replace(
                QString::fromLocal8Bit("’"), "'");
    auto sentenceCN = this->data->at(this->idx).value("sentenceCN");
    ui->enTextEdit->setText(content + "\n" + sentenceCN);

    QUrlQuery postData;
    postData.addQueryItem("content", content);
    postData.addQueryItem("accent", "en");
    postData.addQueryItem("speed", "0");
    auto bytes = QUrl::toPercentEncoding(postData.toString().replace(' ', '+'), "=&+");
    // QByteArray data = postData.toString(QUrl::FullyEncoded).toUtf8();

    QNetworkRequest request;
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    conf.setProtocol(sslProtocol);
    request.setSslConfiguration(conf);
    const static QString postSentenceURL = "https://api.entts.com/post/";
    request.setUrl(QUrl(postSentenceURL));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    request.setRawHeader("accept", "*/*");
    request.setHeader(QNetworkRequest::UserAgentHeader, "curl/7.68.0");
    request.setHeader(QNetworkRequest::ContentLengthHeader, bytes.count());

    QNetworkReply *reply = manager.post(request, bytes);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        const static QRegularExpression sourceRE("<source src=\"([^\"]+)\" type=\"audio/mpeg\">");
        QRegularExpressionMatch match = sourceRE.match(reply->readAll());
        reply->close();
        if (match.hasMatch()) {
            this->audioUrl = match.captured(1);
            playAudio(this->audioUrl);
            ui->playPushBtn->setChecked(true);
            ui->playPushBtn->setText("||");
        }});
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, [=](QNetworkReply::NetworkError code){ qDebug() << "getAudioUrl Error: " << code; });
}

void MainWindow::on_playPushBtn_toggled(bool checked) {
    ui->playPushBtn->setText(checked ? "||" : "i>");
    if (checked) {
        if (player->isAudioAvailable()) {
            player->play();
        }
    } else {
        if (player->isAudioAvailable()) {
            player->pause();
        }
    }
}


bool MainWindow::playAudio(const QString &url) {
    // https://forum.qt.io/topic/9481/qurl-and-special-characters/3
    if (url.startsWith("http")) {
        player->setMedia(QUrl(url));
    } else {
        player->setMedia(QUrl::fromLocalFile(url));
    }
    // cdb: https://www.microsoft.com/en-us/download/details.aspx?id=8279
    // DirectShowPlayerService::doRender: Unresolved error code 80040266
    // http://www.codecguide.com/download_k-lite_codec_pack_standard.htm
    qDebug() << url;
    player->play();
    return true;
}


void MainWindow::on_parseDone(const DataType &data) {
    qDebug() << "Total: " << data.count();
    if (data.count()) {
        this->data = &data;
        ui->nextPushBtn->setEnabled(true);
    }
}

void MainWindow::on_wordClicked(const QVector<QStringList> &dictData) {
    dictModel->setInfo(dictData);
}


void MainWindow::on_nextPushBtn_clicked() {
    if (this->idx < this->data->count()) {
        ++this->idx;
        if (this->idx + 1 == this->data->count()) {
            ui->nextPushBtn->setEnabled(false);
        }
        getAudioUrl();
        if (this->idx == 1) {
            ui->prevPushBtn->setEnabled(true);
        }
    }
}

void MainWindow::on_prevPushBtn_clicked() {
    if (this->idx > -1) {
        --this->idx;
        if (this->idx == 0) {
            ui->prevPushBtn->setEnabled(false);
        }
        getAudioUrl();
    }
}

void MainWindow::on_dictTableView_clicked(const QModelIndex &index)
{
    auto audio = index.sibling(index.row(), 1).data().toString();
    if (!audio.isEmpty()) {
        const static QString pronUrl("https://media.merriam-webster.com/audio/prons/en/us/mp3");
        auto url = QString("%1/%2/%3.mp3").arg(pronUrl).arg(audio.mid(0, 1)).arg(audio);
        qDebug() << url;
        pronPlayer->setMedia(QUrl("https://media.merriam-webster.com/audio/prons/en/us/mp3/s/scrawn01.mp3"));
        pronPlayer->play();
    }
}
