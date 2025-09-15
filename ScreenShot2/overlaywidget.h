#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QWidget>
#include <QRubberBand>

class OverlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OverlayWidget(QWidget *parent = nullptr);
    QRect selectedRect()const;

signals:
     void regionSelected(QRect rect);

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
    bool selecting;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};



#endif // OVERLAYWIDGET_H
