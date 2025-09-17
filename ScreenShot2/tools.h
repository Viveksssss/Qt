#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QWidget>

class QClipboard;
class Snipater;
class Tools : public QWidget
{
    Q_OBJECT
public:
    explicit Tools(Snipater*,QWidget*parent =nullptr);

signals:
    void saveRequested(const QPixmap &pixmap);
    void finishRequested(const QPixmap &pixmap);
    void closeRequested();
    void closeWindow();
private slots:
    void onSaveClicked();
    void onCloseClicked();
    void onFinishClicked();
public slots:
    void setPixmap(const QPixmap pix);
private:
    void createButtons();
    QPixmap _pixmap;
    QWidget*_buttonContainer;
    QClipboard*_clipboard;
    Snipater*sni;

};

#endif // TOOLS_H
