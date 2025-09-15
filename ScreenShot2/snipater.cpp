#include "snipater.h"
#include <QDebug>
#include <QString>
#include <QDir>
#include <QMenu>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QCursor>
#include <QShortcut>
#include <QClipboard>

Snipater::Snipater(QObject *parent)
    : QObject{parent},
    sys(new QSystemTrayIcon(this)),
    view(new Widget)
{
    initSysMenu();

    this->sys->setIcon(QIcon("D:\\Code\\Qt\\ScreenShot2\\resources\\icon.png"));
    this->sys->show();

    overlay = new OverlayWidget;
    clipboard = QApplication::clipboard();

    screenshotShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_E), this);
    connect(screenshotShortcut, &QShortcut::activated, this, &Snipater::grapScreen);


    connect(this,&Snipater::shotFinished,view,&Widget::showPix);
}

void Snipater::copyToClipboard(const QPixmap &pixmap)
{
    clipboard->setPixmap(pixmap);
}

void Snipater::grapScreen()
{
    connect(overlay,&OverlayWidget::regionSelected,this,&Snipater::grabRegion);

    overlay->show();
}

void Snipater::grabRegion(QRect region)
{
    auto screens = QApplication::screens();
    if(screens.empty()){
        QMessageBox::critical(nullptr,"Error","No Useful Screen Devices!");
        return;
    }

    QPixmap pix;
    QPoint center = region.center();

    for(auto& screen : screens){
        QRect screenRect = screen->geometry();
        if(screenRect.contains(center)){
            // 截取指定区域
            pix = screen->grabWindow(0,
                                   region.topLeft().x(),
                                   region.topLeft().y(),
                                   region.width(),
                                   region.height());
            break;
        }
    }

    if(pix.isNull()){
        QMessageBox::critical(nullptr,"Error","Can't Get Screen Region");
        return;
    }

    copyToClipboard(pix);

    emit shotFinished(pix);
}

void Snipater::initSysMenu()
{
    QMenu*menu = new QMenu;
    auto snapAction = menu->addAction("Snapshot(ALT+A)");
    auto quitAction = menu->addAction("Quit");

    connect(snapAction,&QAction::triggered,this,&Snipater::grapScreen);
    connect(quitAction,&QAction::triggered,this,&QApplication::quit);

    this->sys->setContextMenu(menu);

}
