#ifndef CONTENTEDITOR_H
#define CONTENTEDITOR_H

#include <QLineEdit>
#include <QWidget>

class ContentEditor : public QLineEdit
{
    Q_OBJECT
public:
    ContentEditor(QWidget*parent = nullptr);
    ~ContentEditor() = default;

protected:
    void keyPressEvent(QKeyEvent*event)override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void contextMenuEvent(QContextMenuEvent*event) override;
signals:
    void GetContent(QString str);
};

#endif // CONTENTEDITOR_H
