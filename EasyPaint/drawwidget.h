#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QPen>
#include <QFont>
#include <QBrush>
#include <QOpenGLWidget>
#include <QWidget>
#include <QMouseEvent>
#include "staticdata.h"
#include "shape.h"
#include "contenteditor.h"

enum class RotateType{
    ROTATE_0        =0,
    ROTATE_90       =1,
    ROTATE_180      =2,
    ROTATE_270      =3
};


class DrawWidget : public QOpenGLWidget
{
public:
    DrawWidget(QWidget*parent = nullptr);
    ~DrawWidget();
private:
    ContentEditor m_ContentEditor;


    StaticData*m_pStaticData;

    QFont m_Font;
    QPen m_Pen;
    QBrush m_Brush;

    bool m_BtnClicked;
    bool m_IsDrawing;
    ShapeType m_ShapeType;

    QPointF m_MovePoint;
    QPointF m_ClickPoint;

    double m_Scaling;
    RotateType m_RotateType;

    bool m_Clear ;

public:
    inline void setShapeType(const ShapeType&shape){m_ShapeType = shape;}
    int RotateLeft();
    int RotateRight();
    void reset();
    void clear();
    inline void m_ScalingIn(const double &r){m_Scaling*=1.15;update();}
    inline void m_ScalingOut(const double &r){m_Scaling/=1.15;update();}

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent*event)override;
    void resizeEvent(QResizeEvent*event)override;
    void mouseMoveEvent(QMouseEvent*event)override;
    void wheelEvent(QWheelEvent *event) override;
protected slots:
    int fn_Recv_Content(const QString&str);

    // QWidget interface
private:
    QPointF physicalToLogical(const QPointF& physicalPos)const;
    QPointF logicalToPhysical(const QPointF& logicalPos) const ;
};

#endif // DRAWWIDGET_H
