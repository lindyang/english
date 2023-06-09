#ifndef ENTEXTBROWSER_H
#define ENTEXTBROWSER_H
#include <QTextEdit>
#include <QNetworkAccessManager>

class EnTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    EnTextEdit(QWidget *parent=nullptr);

signals:
    void wordClicked(const QString& word);

protected:
    void mousePressEvent(QMouseEvent *mouseEvent);

private:
    void connHost();
    void getHTML(const QString& word);
    void parseHTML(const QString& html);

    QNetworkAccessManager manager;
};

#endif // ENTEXTBROWSER_H
