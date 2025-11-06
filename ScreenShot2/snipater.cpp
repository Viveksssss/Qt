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
#include <QPainter>




Snipater::Snipater(QObject *parent)
    : QObject{parent},
    sys(new QSystemTrayIcon(this)),
    view(new Widget)
{

    // initSysMenu();

    // this->sys->setIcon(QIcon(":/resources/icon.png"));
    // this->sys->show();

    overlay = new OverlayWidget;
    clipboard = QApplication::clipboard();

    screenshotShortcut = new QShortcut(QKeySequence(Qt::ALT|Qt::Key_A), this);
    connect(screenshotShortcut, &QShortcut::activated, this, &Snipater::grapScreen);
    connect(this,&Snipater::shotFinished,view,&Widget::showPix);

    toolBar = new Tools(this,view);
    connect(overlay,&OverlayWidget::showTool,this,[this](){
        showTools(pix);
    });
    connect(toolBar,&Tools::closeWindow,overlay,[](){
        QCoreApplication::quit();
    });
    grapScreen();
}



Snipater::~Snipater()
{
    delete toolBar;
}

void Snipater::copyToClipboard(const QPixmap &pixmap)
{
    clipboard->setPixmap(pixmap);
    QApplication::processEvents();
}

void Snipater::grapScreen()
{
    connect(overlay,&OverlayWidget::regionSelected,this,&Snipater::grabRegion);
    overlay->setStatus(STATUS::NONE);
    overlay->show();
}

// QPixmap Snipater::grabRegion(QRect region)
// {
//     if (toolBar && toolBar->isVisible()) {
//         toolBar->hide();
//         QApplication::processEvents();
//     }

//     auto screens = QApplication::screens();
//     if(screens.empty()){
//         QMessageBox::critical(nullptr, "Error", "No Useful Screen Devices!");
//         return QPixmap();
//     }

//     QPixmap pix;
//     bool regionCaptured = false;

//     // 查找包含region的屏幕
//     for(auto& screen : screens){
//         QRect screenRect = screen->geometry();

//         // 检查region与屏幕是否有交集
//         if(screenRect.intersects(region)){
//             // 计算在屏幕坐标系中的区域
//             QRect screenRegion = region.intersected(screenRect);
//             QPoint screenOffset = screenRegion.topLeft() - screenRect.topLeft();

//             qreal pixelRatio = screen->devicePixelRatio();

//             // 使用物理像素坐标进行截取
//             pix = screen->grabWindow(0,
//                                      screenOffset.x() * pixelRatio,
//                                      screenOffset.y() * pixelRatio,
//                                      screenRegion.width() * pixelRatio,
//                                      screenRegion.height() * pixelRatio);

//             pix.setDevicePixelRatio(pixelRatio);
//             regionCaptured = true;
//             break;
//         }
//     }

//     if(!regionCaptured || pix.isNull()){
//         QMessageBox::critical(nullptr, "Error", "Can't Get Screen Region");
//         return QPixmap();
//     }

//     this->pix = pix;

//     if (toolBar) {
//         toolBar->setPixmap(pix);
//     }

//     copyToClipboard(pix);

//     if (overlay) {
//         overlay->hide();
//     }

//     return pix;
// }

QPixmap Snipater::grabRegion(QRect region)
{
    qDebug() << "=== grabRegion 调试开始 ===";
    qDebug() << "传入区域:" << region;

    if (toolBar && toolBar->isVisible()) {
        toolBar->hide();
        QApplication::processEvents();
    }

    auto screens = QApplication::screens();
    qDebug() << "屏幕数量:" << screens.size();

    if(screens.empty()){
        QMessageBox::critical(nullptr,"Error","No Useful Screen Devices!");
        return QPixmap();
    }

    // 显示所有屏幕信息
    for(int i = 0; i < screens.size(); ++i) {
        QRect screenRect = screens[i]->geometry();
        qDebug() << "屏幕" << i << ":" << screenRect << "名称:" << screens[i]->name();
    }

    QPixmap pix;
    QPoint center = region.center();
    qDebug() << "区域中心点:" << center;

    bool screenFound = false;
    for(auto& screen : screens){
        QRect screenRect = screen->geometry();
        qDebug() << "检查屏幕:" << screenRect << "是否包含点:" << center << "结果:" << screenRect.contains(center);

        if(screenRect.contains(center)){
            screenFound = true;
            qreal pixelRatio = screen->devicePixelRatio();
            qDebug() << "找到匹配屏幕，设备像素比:" << pixelRatio;

            // 检查区域是否完全在屏幕内
            if (!screenRect.contains(region)) {
                qDebug() << "警告: 截图区域不完全在屏幕内";
                qDebug() << "区域:" << region;
                qDebug() << "屏幕:" << screenRect;
            }

            // 截图
            pix = screen->grabWindow(0,
                                     region.x(),
                                     region.y(),
                                     region.width(),
                                     region.height());

            qDebug() << "截图后 - 是否为空:" << pix.isNull();
            qDebug() << "截图后 - 尺寸:" << pix.size();
            qDebug() << "截图后 - 设备像素比:" << pix.devicePixelRatio();

            pix.setDevicePixelRatio(pixelRatio);
            qDebug() << "设置设备像素比后 - 尺寸:" << pix.size();
            break;
        }
    }

    if(!screenFound) {
        qDebug() << "错误: 没有找到包含中心点的屏幕";
    }

    if(pix.isNull()){
        qDebug() << "错误: 截图为空";
        QMessageBox::critical(nullptr,"Error","Can't Get Screen Region");
        return QPixmap();
    }

    this->pix = pix;

    // 保存截图到文件进行验证
    QString debugFile = QDir::tempPath() + "/snipper_debug_screenshot.png";
    if (pix.save(debugFile, "PNG")) {
        qDebug() << "截图已保存到:" << debugFile;
        QFileInfo fileInfo(debugFile);
        qDebug() << "文件大小:" << fileInfo.size() << "bytes";
        qDebug() << "文件是否存在:" << fileInfo.exists();
    } else {
        qDebug() << "保存截图到文件失败";
    }

    if (toolBar) {
        toolBar->setPixmap(pix);
        qDebug() << "工具栏图片设置完成";
    }

    qDebug() << "准备复制到剪切板...";
    copyToClipboard(pix);
    qDebug() << "复制到剪切板调用完成";

    if (overlay) {
        overlay->hide();
    }

    qDebug() << "=== grabRegion 调试结束 ===";
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
