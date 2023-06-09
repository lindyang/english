#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <QObject>
#include <QNetworkAccessManager>


class QNetworkReply;


class HTMLParser: public QObject
{
    Q_OBJECT
public:
    using DataType = QVector<QMap<QString, QString>>;

    HTMLParser(QString articleIdStr_, QObject *parent=nullptr);

signals:
    void parseDone(const DataType& data);

private:
    void connHost();
    void getHTML();
    const QString parseHTML(QNetworkReply *reply);
    void parseParas(const QString& parasStr);

    QString articleIdStr;
    QNetworkAccessManager manager;
    DataType data;
};

#endif // HTMLPARSER_H
