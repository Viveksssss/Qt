#ifndef SHAPE_H
#define SHAPE_H
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

enum class ShapeType{
    SHAPE_UNKNOW            = 0,
    SHAPE_RECTANGLE         = 1,
    SHAPE_ELLIPSE           = 2,
    SHAPE_TRIANGLE          = 3,
    SHAPE_LINE              = 4,
    SHAPE_TEXT              = 5,
    SHAPE_CURSOR            = 6,
};

class Shape
{
public:
    Shape();
    virtual ~Shape() = default;
public:
    inline void setShapeType(const ShapeType&shapeType){this->m_ShapeType = shapeType;}
    inline const ShapeType&getShapeType(){return m_ShapeType;}
    virtual  QJsonObject toJson() const = 0;
    virtual  bool fromJson(const QJsonObject& json) = 0;
private:
    ShapeType m_ShapeType;
};

#endif // SHAPE_H
