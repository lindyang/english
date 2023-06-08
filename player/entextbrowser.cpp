#include "entextbrowser.h"
#include <QMouseEvent>
#include <QDebug>


EnTextBrowser::EnTextBrowser(QWidget *parent)
    : QTextBrowser(parent)
{

}

void EnTextBrowser::mousePressEvent(QMouseEvent *mouseEvent) {
    if (Qt::LeftButton == mouseEvent->button()) {
        QTextCursor textCursor = cursorForPosition(mouseEvent->pos());
        textCursor.select(QTextCursor::WordUnderCursor);
        setTextCursor(textCursor);
        QString word = textCursor.selectedText();
        if (!word.isEmpty()) {
            emit wordClicked(word);
        }
    }
    QTextBrowser::mousePressEvent(mouseEvent);
}
