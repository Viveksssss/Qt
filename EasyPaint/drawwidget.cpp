#include "drawwidget.h"
#include <QPainter>
#include <QDebug>
DrawWidget::DrawWidget(QWidget *parent)
:QOpenGLWidget(parent)
{

    setMouseTracking(true);

    m_pStaticData = StaticData::GetStaticData();

    m_Pen.setColor(QColor(DEFAULT_SHAPE_COLOR));
    m_Font.setFamily(QStringLiteral("楷体"));
    // m_Brush.setStyle(Qt::BrushStyle::SolidPattern);
    m_Brush.setStyle(Qt::BrushStyle::NoBrush);

    m_BtnClicked = false;
    m_IsDrawing = false;
    m_ShapeType = ShapeType::SHAPE_UNKNOW;

    m_MovePoint = {0,0};
    m_ClickPoint = {0,0};

    m_ContentEditor.setParent(this);
    m_ContentEditor.hide();

    m_Scaling = 1;

    m_RotateType = RotateType::ROTATE_0;

    m_Clear = false;
    connect(&m_ContentEditor,&ContentEditor::GetContent,this,&DrawWidget::fn_Recv_Content);
}

DrawWidget::~DrawWidget()
{

}

int DrawWidget::RotateLeft()
{
    int k = static_cast<int>(m_RotateType)-1;
    if(k<0)
        m_RotateType = static_cast<RotateType>(4 - qAbs(k)%4);
    else{
        m_RotateType = static_cast<RotateType>(k%4);
    }
    update();
    return NORMAL_RETURN;
}

int DrawWidget::RotateRight()
{
    m_RotateType = static_cast<RotateType>(qAbs(static_cast<int>(m_RotateType)+1)%4);
    update();
    return NORMAL_RETURN;
}

void DrawWidget::reset()
{
    m_Scaling = 1;
    update();
    int k = (qAbs(static_cast<int>(m_RotateType)));
    for(int i = 0;i<k;i++){
        RotateLeft();
    }
}

void DrawWidget::clear()
{
    m_Clear = true;
    update();
}

void DrawWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_BtnClicked = true;
        m_ClickPoint = physicalToLogical(QPointF(event->pos()));
    }
    QOpenGLWidget::mousePressEvent(event);
}

void DrawWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_BtnClicked = false;
        m_MovePoint = physicalToLogical(event->pos());
        switch(m_ShapeType){
            case ShapeType::SHAPE_RECTANGLE:{
                int x1 = std::min( m_ClickPoint.x() , m_MovePoint.x() );
                int y1 = std::min( m_ClickPoint.y() , m_MovePoint.y() );
                QPointF dis = m_MovePoint - m_ClickPoint;
                Rectangle *rec= new Rectangle(double(x1),double(y1),qAbs(double(dis.x())),qAbs(double(dis.y())));
                rec->setShapeType(ShapeType::SHAPE_RECTANGLE);
                m_pStaticData->m_ShapeVec.push_back(rec);
                update();
                break;
            }
            case ShapeType::SHAPE_ELLIPSE:{
                int x1 = std::min( m_ClickPoint.x() , m_MovePoint.x() );
                int y1 = std::min( m_ClickPoint.y() , m_MovePoint.y() );
                QPointF dis = m_MovePoint - m_ClickPoint;
                Ellipse *ellipse = new Ellipse(double(x1),double(y1),qAbs(dis.x()),qAbs(dis.y()));
                ellipse->setShapeType(ShapeType::SHAPE_ELLIPSE);
                m_pStaticData->m_ShapeVec.push_back(ellipse);
                update();
                break;
            }
            case ShapeType::SHAPE_TRIANGLE:{
                int x1 = std::min( m_ClickPoint.x() , m_MovePoint.x() );
                int y1 = std::max( m_ClickPoint.y() , m_MovePoint.y() );
                QPointF dis = m_MovePoint - m_ClickPoint;
                Triangle *tri = new Triangle(double(x1),double(y1),qAbs(dis.x()),qAbs(dis.y()));
                tri->setShapeType(ShapeType::SHAPE_TRIANGLE);
                m_pStaticData->m_ShapeVec.push_back(tri);
                update();
                break;
            }
            case ShapeType::SHAPE_LINE:{
                Line*line = new Line(m_ClickPoint.x(),m_ClickPoint.y(),m_MovePoint.x(),m_MovePoint.y());
                line->setShapeType(ShapeType::SHAPE_LINE);
                m_pStaticData->m_ShapeVec.push_back(line);
                update();
                break;
            }
            case ShapeType::SHAPE_TEXT:{
                m_ContentEditor.clear();
                m_ContentEditor.show();
                QPointF pos = logicalToPhysical((m_MovePoint));
                m_ContentEditor.setGeometry(pos.x(),pos.y(),200,30);
                m_ContentEditor.setFocus();
                break;
            }
            default:{

                break;
            }
        }
    }
    m_IsDrawing = false;
    QOpenGLWidget::mouseReleaseEvent(event);
}

void DrawWidget::paintEvent(QPaintEvent *event)
{
    QOpenGLWidget::paintEvent(event);
    glClearColor(255,255,255,0);
    if(m_Clear == true){
        m_Clear = false;
        StaticData::GetStaticData()->getVec().clear();
        return;
    }

    QPainter painter(this);
    painter.scale(m_Scaling,m_Scaling);
    painter.setFont(m_Font);
    painter.setBrush(m_Brush);
    painter.setPen(m_Pen);

    switch(m_RotateType){
        case RotateType::ROTATE_90:{
            painter.translate(width(),0.0);
            painter.rotate(90);
            break;
        }
        case RotateType::ROTATE_180:{
            painter.translate(width(),height());
            painter.rotate(180);
            break;
        }
        case RotateType::ROTATE_270:{
            painter.translate(0.0,height());
            painter.rotate(270);
            break;
        }
        default:{
            painter.translate(0.0,0.0);
            painter.rotate(0);
            break;
        }
    }


    qDebug() << m_pStaticData->m_ShapeVec.size();
    for (auto* shape : m_pStaticData->m_ShapeVec) {
        // 先做安全类型检查
        if (!shape) continue;

        switch(shape->getShapeType()) {
            case ShapeType::SHAPE_RECTANGLE: {
                if (auto* rect = dynamic_cast<Rectangle*>(shape)) {
                    painter.drawRect(QRectF(
                        rect->getStartX(),
                        rect->getStartY(),
                        rect->getWidth(),
                        rect->getHeight()
                    ));
                }
                break;
            }
            case ShapeType::SHAPE_ELLIPSE:{
                if(auto*ellipse = dynamic_cast<Ellipse*>(shape)){
                    painter.drawEllipse(QRect(
                        ellipse->getStartX(),
                        ellipse->getStartY(),
                        ellipse->getRadius1(),
                        ellipse->getRadius2()
                    ));
                }
                break;
            }
            case ShapeType::SHAPE_TRIANGLE:{
                if(auto*ellipse = dynamic_cast<Triangle*>(shape)){
                    QPointF p1(ellipse->getStartX(),ellipse->getStartY() );
                    QPointF p2(ellipse->getStartX()+ellipse->getWidth(),ellipse->getStartY());
                    QPointF p3(ellipse->getStartX()+ellipse->getWidth()/2.0,ellipse->getStartY() - ellipse->getHeight());
                    painter.drawLine(p1,p2);
                    painter.drawLine(p2,p3);
                    painter.drawLine(p3,p1);
                }
                break;
            }
            case ShapeType::SHAPE_LINE:{
                if(auto*line = dynamic_cast<Line*>(shape)){
                    painter.drawLine(line->getStartX(),line->getStartY(),line->getEndX(),line->getEndY());
                }
                break;
            }
            case ShapeType::SHAPE_TEXT:{
                if(auto*text = dynamic_cast<Text*>(shape)){
                    QPointF pos = (QPointF(text->getStartX(),text->getStartY()));
                    painter.drawText(QPointF(pos.x(),pos.y() ),text->getContent());
                }
                break;
            }

            default:
                qWarning() << "Unhandled shape type or type mismatch";
                break;
        }
    }
        switch(m_ShapeType){
            case ShapeType::SHAPE_RECTANGLE:{
                if(m_IsDrawing == true && qAbs((m_MovePoint-m_ClickPoint).y() - (m_MovePoint-m_ClickPoint).x()) <3){
                    QPen pen = m_Pen;
                    pen.setWidth(pen.width()+5);
                    painter.setPen(pen);
                }
                painter.drawRect(QRectF(m_ClickPoint,m_MovePoint));
                break;
            }
            case ShapeType::SHAPE_ELLIPSE:{
                if(m_IsDrawing == true && qAbs((m_MovePoint-m_ClickPoint).y() - (m_MovePoint-m_ClickPoint).x()) <3){
                    QPen pen = m_Pen;
                    pen.setWidth(pen.width()+5);
                    painter.setPen(pen);
                }
                painter.drawEllipse(QRectF(m_ClickPoint,m_MovePoint));
                break;
            }
            case ShapeType::SHAPE_TRIANGLE:{
                if(m_IsDrawing == true && qAbs((m_MovePoint-m_ClickPoint).y() - (m_MovePoint-m_ClickPoint).x()) <3){
                    QPen pen = m_Pen;
                    pen.setWidth(pen.width()+5);
                    painter.setPen(pen);
                }
                int x1 = std::min( m_ClickPoint.x() , m_MovePoint.x() );
                int y1 = std::max( m_ClickPoint.y() , m_MovePoint.y() );
                QPointF dis = m_MovePoint - m_ClickPoint;
                painter.drawLine(QPointF(x1,y1),QPointF(x1+qAbs(dis.x()),y1));
                painter.drawLine(QPointF(x1,y1),QPointF(x1+qAbs(dis.x())/2.0,y1-qAbs(dis.y())));
                painter.drawLine(QPointF(x1+qAbs(dis.x())/2.0,y1-qAbs(dis.y())),QPointF(x1+qAbs(dis.x()),y1));
                break;
            }
            case ShapeType::SHAPE_LINE:{
                painter.drawLine(m_ClickPoint.x(),m_ClickPoint.y(),m_MovePoint.x(),m_MovePoint.y());
                break;
            }
            default:{
                break;
            }
        }
}

void DrawWidget::resizeEvent(QResizeEvent *event)
{
    QOpenGLWidget::resizeEvent(event);
}


void DrawWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_BtnClicked == true){
        if(m_ShapeType!=ShapeType::SHAPE_UNKNOW && m_ShapeType!=ShapeType::SHAPE_CURSOR){
            m_IsDrawing = true;
        }
        m_MovePoint =physicalToLogical(event->pos());
        update();
    }
    QOpenGLWidget::mouseMoveEvent(event);
}

void DrawWidget::wheelEvent(QWheelEvent *event)
{
    const double zoomFactor = 1.15;
    qDebug() <<"zoom" <<":" << m_Scaling;

    if(event->angleDelta().y()>0){
        m_Scaling*=zoomFactor;
    }else{
        m_Scaling/=zoomFactor;
    }
    update();
}

int DrawWidget::fn_Recv_Content(const QString &str)
{
    Text*text = new Text(m_MovePoint.x(),m_MovePoint.y(),str);
    text->setShapeType(ShapeType::SHAPE_TEXT);
    m_pStaticData->m_ShapeVec.push_back(text);
    update();
    return NORMAL_RETURN;
}

QPointF DrawWidget::physicalToLogical(const QPointF &physicalPos) const
{
    return QPointF(
            physicalPos.x() / m_Scaling,
            physicalPos.y() / m_Scaling
    );
}
QPointF DrawWidget::logicalToPhysical(const QPointF& logicalPos) const {
    return QPointF(
        logicalPos.x() * m_Scaling,
        logicalPos.y() * m_Scaling
    );
}
