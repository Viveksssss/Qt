#include "overlaywidget.h"
#include <QApplication>
#include <QScreen>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

OverlayWidget::OverlayWidget(QWidget *parent)
    : QWidget{parent}
{

    setMouseTracking(true); // 必须启用这个！
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);


    QScreen*screen = QGuiApplication::primaryScreen();
    setGeometry(screen->geometry());
}

QRect OverlayWidget::selectedRect() const
{
    return QRect(startPoint,endPoint).normalized();
}

QRect OverlayWidget::dragRect() const
{
    return selectedRect().translated(currentDragOffset);
}

void OverlayWidget::setStatus(STATUS status)
{
    this->status = status;
}


void OverlayWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape){
        hide();
    }else if(event->key() == Qt::Key_Return){
        emit regionSelected(selectedRect());
        hide();
    }
    QWidget::keyPressEvent(event);
}

void OverlayWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && status != STATUS::AFTER){
        startPoint = event->pos();
        endPoint = startPoint;
    }
    if(status == STATUS::NONE){
        status = STATUS::SELECTING;
    }else if(status == STATUS::AFTER){
        QRect rect = selectedRect();
        if(rect.contains(event->pos())){
            status = STATUS::DRAGGING;
            dragStart = event->pos();
            currentDragOffset = QPoint(0, 0); // 重置偏移
        }
    }
}

void OverlayWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        if(status == STATUS::SELECTING){
            endPoint = event->pos();
            if (selectedRect().width() > 5 && selectedRect().height() > 5) {
                status = STATUS::AFTER;
                emit showTool();
            } else {
                status = STATUS::NONE;
            }
        }
        else if(status == STATUS::DRAGGING){
            startPoint += currentDragOffset;
            endPoint += currentDragOffset;

            status = STATUS::AFTER;
        }
    }
    update();
}

// CPP
// Copy Paste Programming

void OverlayWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(status == STATUS::SELECTING ){
        endPoint = event->pos();
        setCursor(Qt::CrossCursor);
    }else if(status == STATUS::DRAGGING){
        currentDragOffset = event->pos() - dragStart;
        setCursor(Qt::ClosedHandCursor);
    }else if(status == STATUS::AFTER){
        QRect finalRect = selectedRect();
        if(finalRect.contains(event->pos())){
            setCursor(Qt::SizeAllCursor);
        }else{
            setCursor(Qt::ArrowCursor);
        }
    }
    update();
}

void OverlayWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.fillRect(rect(),QColor(0,0,0,100));
    painter.setPen(QPen(Qt::cyan,2));
    painter.drawRect(rect());
    if(status == STATUS::SELECTING || status == STATUS::AFTER){
        QRect selection = selectedRect();

        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.fillRect(selection, Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

        painter.setPen(QPen(Qt::red,2));
        painter.drawRect(selection.x()-1,selection.y()-1,selection.width()+3,selection.height()+3);
    }else if(status == STATUS::DRAGGING){
        QRect selection = dragRect();

        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.fillRect(selection, Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

        painter.setPen(QPen(Qt::red,2));
        painter.drawRect(selection.x()-1,selection.y()-1,selection.width()+3,selection.height()+3);
    }
}


void OverlayWidget::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

