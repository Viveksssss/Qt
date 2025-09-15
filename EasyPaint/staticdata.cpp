#include "staticdata.h"
#include "drawwidget.h"


StaticData *StaticData::GetStaticData()
{
    static StaticData s_StaticData;
    return &s_StaticData;
}

Shape *StaticData::createFromJson(const QJsonObject &json)
{
       if (!json.contains("type")) return nullptr;

       QString type = json["type"].toString();
       Shape* shape = nullptr;

       if (type == "rectangle"){
           shape = new Rectangle();
           shape->setShapeType(ShapeType::SHAPE_RECTANGLE);
       }
       else if (type == "ellipse"){
           shape = new Ellipse();
           shape->setShapeType(ShapeType::SHAPE_ELLIPSE);
       }
       else if (type == "triangle"){
           shape = new Triangle();
           shape->setShapeType(ShapeType::SHAPE_TRIANGLE);
       }
       else if (type == "line"){
           shape = new Line();
           shape->setShapeType(ShapeType::SHAPE_LINE);
       }
       else if (type == "text"){
           shape = new Text();
           shape->setShapeType(ShapeType::SHAPE_TEXT);
       }

       if (!shape || !shape->fromJson(json)) {
           delete shape;
           return nullptr;
       }
       return shape;
}

StaticData::StaticData()
{

}

StaticData::~StaticData()
{
    qDeleteAll<QVector<Shape*>>(m_ShapeVec);
    m_ShapeVec.clear();
    m_ShapeVec.shrink_to_fit();
}
