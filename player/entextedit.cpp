#include "entextedit.h"
#include "requests.h"

#include <QMouseEvent>
#include <QDebug>

#include <QMediaPlayer>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QRegularExpressionMatch>


EnTextEdit::EnTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    connHost();
}


void EnTextEdit::connHost() {
    const static QString zhiHuHost = "www.merriam-webster.com";
    manager.connectToHostEncrypted(zhiHuHost, 443, getSslConf());
}

// https://media.merriam-webster.com/audio/prons/en/us/mp3/e/eleven01.mp3

void EnTextEdit::getHTML(const QString& word) {
    const QString merriamWebsterUrl = "https://www.merriam-webster.com/dictionary/%1";
    auto url = merriamWebsterUrl.arg(word);
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply](){
//        QString html = reply->read(451500+1000).mid(451500);
        QString html = reply->readAll();
        reply->close();
        if (!html.isEmpty()) {
            parseHTML(html);
        }
    });
}

void EnTextEdit::parseHTML(const QString& html) {
    const static QString start = "<div id=\"dictionary-entry-1\" class=\"entry-word-section-container\">";
    const static QString end = "<span id=\"anchor-seporator\"></span>";
    int startIdx, endIdx;
    if ((startIdx = html.indexOf(start, 440000)) == -1 || (endIdx = html.indexOf(end, startIdx)) == -1) {
        return;
    }

    auto divStr = html.mid(startIdx, endIdx - startIdx);
    QVector<QStringList> dictData;
    QStringList firstItem;

    static QRegularExpression syllableRe(" +<span class=\"word-syllables-entry\">([^ ]+)</span>");
    auto syllableMatch = syllableRe.match(divStr);
    if (syllableMatch.hasMatch()) {
        auto syllable = syllableMatch.captured(1).replace("&#8203;", "");
        auto conjugate = QString(syllable).replace("·", "");
        firstItem << syllable;  // --
    } else {
        static QRegularExpression wordRe(" +<h1 class=\"hword\">([^ ]+)</h1>");
        auto wordMatch = wordRe.match(divStr);
        if (wordMatch.hasMatch()) {
            auto conjugate = wordMatch.captured(1);
            firstItem << conjugate;
        } else {
            firstItem << "";
        }
    }

    static QRegularExpression mainTupleRe(
        "\\s+<span .+>\\n\\s+<a .+ data-file=\"(\\w+)\" .+ \\(audio\\)\">([^ ]+)&nbsp;<img ",
        QRegularExpression::MultilineOption);

    QRegularExpressionMatch mainMatch = mainTupleRe.match(divStr);
    if (mainMatch.hasMatch()) {
        auto audio = mainMatch.captured(1);
        auto phonetic = mainMatch.captured(2);
        firstItem << audio << phonetic;
    } else {
        firstItem << "" << "";
    }
    // ---------------------
    dictData.append(firstItem);
    // ---------------------

    static QRegularExpression remainingTupleRe(
        "<span class=\"(?:fw-bold ure|if)\">([^ ]+)</span>\\n\\s+<(?:div|span) .+>\\n\\s+<a .+ data-file=\"(\\w+)\" .+ \\(audio\\)\">([^ ]+)&nbsp;<img ",
        QRegularExpression::MultilineOption);

    QRegularExpressionMatchIterator matchIter = remainingTupleRe.globalMatch(divStr);
    while (matchIter.hasNext()) {
        QRegularExpressionMatch match = matchIter.next();
        QString conjugate = match.captured(1);
        QString audio = match.captured(2);
        QString phonetic = match.captured(3);
        dictData.append(QStringList{conjugate, audio, phonetic});
    }
    emit lookUpDone(dictData);  // send signal
}

void EnTextEdit::mousePressEvent(QMouseEvent *mouseEvent) {
    if (Qt::LeftButton == mouseEvent->button()) {
        QTextCursor textCursor = cursorForPosition(mouseEvent->pos());
        textCursor.select(QTextCursor::WordUnderCursor);
        setTextCursor(textCursor);
        QString word = textCursor.selectedText();
        if (!word.isEmpty()) {
            getHTML(word);
        }
    }
    QTextEdit::mousePressEvent(mouseEvent);
}
