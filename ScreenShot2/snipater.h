#ifndef SNIPATER_H
#define SNIPATER_H

#include <QObject>
#include <QSystemTrayIcon>
#include "widget.h"
#include "overlaywidget.h"


class QShortcut;
class Snipater : public QObject
{
    Q_OBJECT
public:
    explicit Snipater(QObject *parent = nullptr);
    void copyToClipboard(const QPixmap&pixmap);
public slots:
    void grapScreen();
    void grabRegion(QRect region);

signals:
    void shotFinished(QPixmap);
private:
    void initSysMenu();
    QSystemTrayIcon*sys;
    Widget*view;
    OverlayWidget*overlay;
    QClipboard*clipboard ;
    QShortcut*screenshotShortcut;
};

#endif // SNIPATER_H
