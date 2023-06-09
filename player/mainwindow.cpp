#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "htmlparser.h"
#include <requests.h>

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

    ui->enTextEdit->setFont(QFont("Times", 16, QFont::Bold));
    connect(ui->enTextEdit, SIGNAL(wordClicked(const QString&)), SLOT(on_wordClicked(const QString&)));

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
}

void MainWindow::getAudioUrl() {
    auto sentence = this->data->at(this->idx).value("sentence");
    ui->enTextEdit->setText(sentence);

    QUrlQuery postData;
    postData.addQueryItem("content", sentence);
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
            this, [=](QNetworkReply::NetworkError code){ qDebug() << "Error: " << code; });
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
    if (url.startsWith("http")) {
        player->setMedia(QUrl(url));
    } else {
        player->setMedia(QUrl::fromLocalFile(url));
    }
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

void MainWindow::on_wordClicked(const QString& word) {
    qDebug() << word;
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
