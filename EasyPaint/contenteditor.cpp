#include "contenteditor.h"
#include <QKeyEvent>

ContentEditor::ContentEditor(QWidget *parent):
    QLineEdit(parent)
{
    this->setStyleSheet("background-color:white;");
}

void ContentEditor::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key::Key_Return){
        hide();
        this->parentWidget()->setFocus();
        emit GetContent(text());
    }
    else if(event->key() == Qt::Key::Key_Escape){
        hide();
        this->parentWidget()->setFocus();
    }
    QLineEdit::keyPressEvent(event);
}

void ContentEditor::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    selectAll();
}

void ContentEditor::focusOutEvent(QFocusEvent *event)
{
    hide();
    // emit GetContent(text());
    QLineEdit::focusOutEvent(event);
}

void ContentEditor::contextMenuEvent(QContextMenuEvent *event)
{
    event->ignore();
}
