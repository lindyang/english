#include "htmlparser.h"
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
    data = new DataType;
    connectZhiHuHost();
    getHTMLContent();
}


void HTMLParser::connectZhiHuHost() {
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
#if defined(Q_OS_LINUX)
    auto protocol = QSsl::TlsV1SslV3;
#else
    auto protocol = QSsl::AnyProtocol;
#endif
    sslConfig.setProtocol(protocol);  // QSsl::TlsV1SslV3, QSsl::AnyProtocol
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);

    const static QString ZHIHUHOST = "zhuanlan.zhihu.com";
    manager.connectToHostEncrypted(ZHIHUHOST, 443, sslConfig);
}


void HTMLParser::getHTMLContent() {
    const static QString zhihuUrl = "https://zhuanlan.zhihu.com/p/%1";  // 619666579
    auto url = zhihuUrl.arg(articleIdStr);
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply](){
        auto parasStr = this->parseHTML(reply);
        if (!parasStr.isEmpty()) {
            parseParas(parasStr);
        }
    });
}


const QString HTMLParser::parseHTML(QNetworkReply *reply) {
    const static QString startTag = "<script id=\"js-initialData\" type=\"text/json\">";
    const static QString endTag = "</script>";

    QRegularExpression jsonRE(QString("%1(.+)%2").arg(startTag, endTag), QRegularExpression::InvertedGreedinessOption);
    QRegularExpressionMatch jsonMatch = jsonRE.match(reply->readAll());
    if (jsonMatch.hasMatch()) {
        QString jsonStr = jsonMatch.captured(1);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());
        QJsonObject jsonObj = jsonDoc.object();
        for (auto key: QStringList{"initialState", "entities", "articles", articleIdStr}) {
            jsonObj = jsonObj.value(key).toObject();
        }
        return jsonObj.value("content").toString();
    }
    return "";
}


void HTMLParser::parseParas(const QString& parasStr) {
    const static QRegularExpression paraRE("<p data-pid=\"([\\w-]+)\">(.+)</p>", QRegularExpression::InvertedGreedinessOption);
    // breathlessness (...) (n.) 呼吸急促(呼吸急促是健康问题征象)
    const static QRegularExpression wordLineRE("([^(]+) ?\\([^)]+\\) ?\\([^)]+\\) ?([^(]+)");

    QRegularExpressionMatchIterator paraIter = paraRE.globalMatch(parasStr);
    paraIter.next();
    paraIter.next();

    while (paraIter.hasNext()) {
        QRegularExpressionMatch matchPara = paraIter.next();
        QString dataId = matchPara.captured(1);
        QString para = matchPara.captured(2);
        para.replace(QRegularExpression("</?[bi]>"), "");
        QStringList lines = para.split("<br/>");
        QString firstLine = lines.at(0);
        QRegularExpressionMatch match = wordLineRE.match(firstLine);
        if (match.hasMatch()) {
//            qDebug() << match.captured(1);
            data->append(new QMap<QString, QString>{
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
