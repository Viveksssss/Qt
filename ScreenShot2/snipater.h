#ifndef SNIPATER_H
#define SNIPATER_H

#include <QObject>
#include <QSystemTrayIcon>
#include "widget.h"
#include "overlaywidget.h"
#include "tools.h"
#include <QMutex>

class QShortcut;
class Snipater : public QObject
{
    Q_OBJECT
public:
    explicit Snipater(QObject *parent = nullptr);
    OverlayWidget*overlay;
    ~Snipater();
    void copyToClipboard(const QPixmap&pixmap);
    void showTools(QPixmap pix);
public slots:
    void grapScreen();
    QPixmap grabRegion(QRect region);
signals:
    void shotFinished(QPixmap);
private:
    void initSysMenu();
    QSystemTrayIcon*sys;
    Widget*view;

    QClipboard*clipboard ;
    QShortcut*screenshotShortcut;
    Tools*toolBar;
    QPixmap pix;
};

#endif // SNIPATER_H
