#include "overlaywidget.h"
#include <QApplication>
#include <QScreen>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

OverlayWidget::OverlayWidget(QWidget *parent)
    : QWidget{parent}
{
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


void OverlayWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape){
        hide();
    }
    QWidget::keyPressEvent(event);
}

void OverlayWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        startPoint = event->pos();
        endPoint = startPoint;
        selecting = true;
    }
}

void OverlayWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && selecting){
        endPoint = event->pos();
        QRect finalRect = selectedRect();

        if(finalRect.width()>5&&finalRect.height()>5){
            emit regionSelected(finalRect);
            hide();
        }
        else{
            update();
        }
        selecting = false;
    }
}

void OverlayWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(selecting){
        endPoint = event->pos();
        update();
    }
}

void OverlayWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.fillRect(rect(),QColor(0,0,0,100));
    painter.setPen(QPen(Qt::cyan,2));
    painter.drawRect(rect());
    if(selecting){
        QRect selection = selectedRect();

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

