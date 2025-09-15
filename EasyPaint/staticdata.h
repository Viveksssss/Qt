#ifndef STATICDATA_H
#define STATICDATA_H

#include "Micro.h"
#include "shape.h"
#include "rectangle.h"
#include "ellipse.h"
#include "triangle.h"
#include "text.h"
#include "line.h"
#include "cursor.h"


#include <QVector>

class DrawWidget;

class StaticData
{
    friend DrawWidget;
public:
    static StaticData*GetStaticData();
    inline QVector<Shape*>&getVec(){return this->m_ShapeVec;}
    static Shape* createFromJson(const QJsonObject& json);
private:
    StaticData();
    ~StaticData();

private:
    // static StaticData s_StaticData;
    QVector<Shape*>m_ShapeVec;

};

#endif // STATICDATA_H
