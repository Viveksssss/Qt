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

    this->sys->setIcon(QIcon(":/resources/icon.png"));
    this->sys->show();

    overlay = new OverlayWidget;
    clipboard = QApplication::clipboard();

    screenshotShortcut = new QShortcut(QKeySequence(Qt::ALT|Qt::Key_A), this);
    connect(screenshotShortcut, &QShortcut::activated, this, &Snipater::grapScreen);
    connect(this,&Snipater::shotFinished,view,&Widget::showPix);

    toolBar = new Tools(this,view);
    connect(overlay,&OverlayWidget::showTool,this,[this](){
        showTools(pix);
    });
    connect(toolBar,&Tools::closeWindow,overlay,&OverlayWidget::hide);
}

Snipater::~Snipater()
{
    delete toolBar;
}

void Snipater::copyToClipboard(const QPixmap &pixmap)
{
    clipboard->setPixmap(pixmap);
}

void Snipater::grapScreen()
{
    connect(overlay,&OverlayWidget::regionSelected,this,&Snipater::grabRegion);
    overlay->setStatus(STATUS::NONE);
    overlay->show();
}

QPixmap Snipater::grabRegion(QRect region)
{
    if (toolBar && toolBar->isVisible()) {
        toolBar->hide();
        QApplication::processEvents();
    }

    auto screens = QApplication::screens();
    if(screens.empty()){
        QMessageBox::critical(nullptr,"Error","No Useful Screen Devices!");
        return QPixmap();
    }

    QPixmap pix;
    QPoint center = region.center();

    for(auto& screen : screens){
        QRect screenRect = screen->geometry();
        if(screenRect.contains(center)){
            qreal pixelRatio = screen->devicePixelRatio(); // 获取设备像素比
            // 截取指定区域
            pix = screen->grabWindow(0,
                                   region.topLeft().x(),
                                   region.topLeft().y(),
                                   region.width(),
                                   region.height());
            pix.setDevicePixelRatio(pixelRatio);
            break;
        }
    }

    if(pix.isNull()){
        QMessageBox::critical(nullptr,"Error","Can't Get Screen Region");
        return QPixmap();
    }

    copyToClipboard(pix);
    this->pix = pix;

    // emit shotFinished(pix);
    return pix;
}

void Snipater::showTools(QPixmap pix)
{
    if(toolBar){
        toolBar->setPixmap(pix);
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - toolBar->width()) / 2;
        int y = screenGeometry.height() - toolBar->height() - 20;

        toolBar->move(x, y);
        toolBar->show();
        toolBar->raise();
        toolBar->activateWindow();
    }
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
