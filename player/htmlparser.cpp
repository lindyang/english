﻿#include "htmlparser.h"
#include <requests.h>

#include <QSsl>
#include <QSslConfiguration>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QJsonDocument>
#include <QJsonObject>


HTMLParser::HTMLParser(QString articleIdStr_, QObject *parent)
    :articleIdStr(articleIdStr_), QObject(parent)
{
    connHost();
    getHTML();
}


void HTMLParser::connHost() {
    const static QString zhiHuHost = "zhuanlan.zhihu.com";
    manager.connectToHostEncrypted(zhiHuHost, 443, getSslConf());
}


void HTMLParser::getHTML() {
    const QString zhihuUrl = "https://zhuanlan.zhihu.com/p/%1";
    auto url = zhihuUrl.arg(articleIdStr);
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply](){
        auto parasStr = this->parseHTML(reply);
        reply->close();
        if (!parasStr.isEmpty()) {
            parseParas(parasStr);
        }
    });
}



const QString HTMLParser::parseHTML(QNetworkReply *reply) {
#ifdef Q_OS_LINUX
    const static QString startTag = "<script id=\"js-initialData\" type=\"text/json\">";
    const static QString endTag = "</script>";
#else
    const static QString startTag = "<div class=\"RichText ztext Post-RichText css-1g0fqss\" options=\"\\[object Object\\]\">";
    const static QString endTag = "</div>";
#endif
    const static QRegularExpression re(QString("%1(.+)%2").arg(startTag, endTag), QRegularExpression::InvertedGreedinessOption);
    QRegularExpressionMatch match = re.match(reply->readAll());
    if (match.hasMatch()) {
        QString content = match.captured(1);
#ifdef Q_OS_LINUX
        QJsonDocument jsonDoc = QJsonDocument::fromJson(content.toUtf8());
        QJsonObject jsonObj = jsonDoc.object();
        for (auto key: QStringList{"initialState", "entities", "articles", articleIdStr}) {
            jsonObj = jsonObj.value(key).toObject();
        }
        return jsonObj.value("content").toString();
#else
        return content;
#endif
    }
    return "";
}


void HTMLParser::parseParas(const QString& parasStr) {
    const static QRegularExpression paraRE("<p data-pid=\"([\\w-]+)\">(.+)</p>", QRegularExpression::InvertedGreedinessOption);
    // breathlessness (...) (n.) 呼吸急促(呼吸急促是健康问题征象)
    const static QRegularExpression wordLineRE("([^(]+) ?\\([^)]+\\) ?\\([^)]+\\) ?([^(]+)");

    QRegularExpressionMatchIterator paraIter = paraRE.globalMatch(parasStr);

    while (paraIter.hasNext()) {
        QRegularExpressionMatch matchPara = paraIter.next();
        QString dataId = matchPara.captured(1);
        QString para = matchPara.captured(2);
        para.replace(QRegularExpression("</?[bi]>"), "");
        QStringList lines = para.split("<br/>");
        QString firstLine = lines.at(0);
        QRegularExpressionMatch match = wordLineRE.match(firstLine);
        if (match.hasMatch()) {
            data.append(QMap<QString, QString>{
                {"dataId", dataId},
                {"word", match.captured(1)},
                {"wordCN", match.captured(2)},
                {"sentence", lines.at(2)},
                {"sentenceCN", lines.at(1)},
            });

        }
    }
    emit parseDone(data);  // send signals
}
