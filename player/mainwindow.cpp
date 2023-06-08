#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "htmlparser.h"

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

    connect(ui->enTextBrowser, SIGNAL(wordClicked(const QString&)), SLOT(on_wordClicked(const QString&)));

    parser = new HTMLParser(articleIdStr, this);
    connect(parser, SIGNAL(parseDone(DataType*)), this, SLOT(on_parseDone(DataType*)));

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
}

void MainWindow::uploadAndGetUrl() {
    QUrlQuery postData;
    auto sentence = this->data->at(this->idx)->value("sentence");

    ui->enTextBrowser->setText(sentence);

    postData.addQueryItem("content", sentence);
    postData.addQueryItem("accent", "en");
    postData.addQueryItem("speed", "0");
    auto bytes = QUrl::toPercentEncoding(postData.toString().replace(' ', '+'), "=&+");
//    QByteArray data = postData.toString(QUrl::FullyEncoded).toUtf8();

    QNetworkRequest request;

    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
#if defined(Q_OS_LINUX)
    auto protocol = QSsl::TlsV1SslV3;
#else
    auto protocol = QSsl::AnyProtocol;
#endif
    conf.setProtocol(protocol);
    request.setSslConfiguration(conf);
    const static QString postSentenceURL = "https://api.entts.com/post/";
    request.setUrl(QUrl(postSentenceURL));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    request.setRawHeader("accept", "*/*");
    request.setHeader(QNetworkRequest::UserAgentHeader, "curl/7.68.0");
    request.setHeader(QNetworkRequest::ContentLengthHeader, bytes.count());
    QNetworkReply *reply = manager.post(request, bytes);

    connect(reply, &QNetworkReply::finished, this, [this, reply](){
//        qDebug() << "upload finished";
        QRegularExpression sourceRE("<source src=\"([^\"]+)\" type=\"audio/mpeg\">");
        QRegularExpressionMatch match = sourceRE.match(reply->readAll());
        if (match.hasMatch()) {
            this->audioUrl = match.captured(1);
            playAudio(this->audioUrl);
            ui->playPushBtn->setChecked(true);
            ui->playPushBtn->setText("||");
        }

    });

    connect(reply,
            static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this,
        [=](QNetworkReply::NetworkError code){
        qDebug() << "upload error: " << code;
    });
}

void MainWindow::on_playPushBtn_toggled(bool checked) {
//    qDebug() << "on_playPushBtn_toggled: " << checked;
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


void MainWindow::on_parseDone(DataType *data) {
    qDebug() << "Total: " << data->count();
    if (data->count()) {
        this->data = data;
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
        uploadAndGetUrl();
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
        uploadAndGetUrl();
    }
}
