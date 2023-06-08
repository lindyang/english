#ifndef ENTEXTBROWSER_H
#define ENTEXTBROWSER_H
#include <QTextBrowser>


class EnTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    EnTextBrowser(QWidget *parent=nullptr);

signals:
    void wordClicked(const QString& word);

protected:
    void mousePressEvent(QMouseEvent *mouseEvent);
};

#endif // ENTEXTBROWSER_H
