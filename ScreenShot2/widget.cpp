#include "widget.h"
#include <QPaintEvent>
#include <QPainter>
#include <QApplication>
Widget::Widget(QWidget *parent)
    : QWidget{parent}
{
}

void Widget::showPix(QPixmap pix)
{
    this->windowPix = pix;
    if (!pix.isNull()) {
        // 根据图片大小计算合适的窗口大小
        QSize screenSize = QApplication::primaryScreen()->size();
        QSize pixSize = pix.size();

        // 保持宽高比，最大不超过屏幕的80%
        double scaleFactor = 0.8;
        int maxWidth = screenSize.width() * scaleFactor;
        int maxHeight = screenSize.height() * scaleFactor;

        if (pixSize.width() > maxWidth || pixSize.height() > maxHeight) {
            // 需要缩放
            pixSize.scale(maxWidth, maxHeight, Qt::KeepAspectRatio);
        }

        // 设置固定大小
        setFixedSize(pixSize);

        // 居中显示
        QRect screenGeometry = QApplication::primaryScreen()->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }

    this->show();
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if(!this->windowPix.isNull()){
        painter.drawPixmap(this->rect(),windowPix);
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
}
