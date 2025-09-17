#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QWidget>
#include <QRubberBand>

enum class STATUS {
    NONE,
    SELECTING,
    DRAGGING,  // 修正拼写为 DRAGGING
    AFTER
};

class OverlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OverlayWidget(QWidget *parent = nullptr);
    QRect selectedRect()const;
    QRect dragRect()const;
    void setStatus(STATUS status);


signals:
    void regionSelected(QRect rect);
    void showTool();

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
private:
    QPoint startPoint;
    QPoint endPoint;
    QPoint dragStart;          // 拖拽开始点
    QPoint currentDragOffset;  // 当前拖拽偏移量

    STATUS status = STATUS::NONE;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};



#endif // OVERLAYWIDGET_H
