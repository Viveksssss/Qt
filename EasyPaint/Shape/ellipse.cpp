#include "ellipse.h"


Ellipse::Ellipse():
    m_StartX(0),
    m_StartY(0),
    m_Radius1(0),
    m_Radius2(0)
{

}

Ellipse::Ellipse(const double &x, const double &y, const double &radius1,const double&radius2)
{
    m_StartX = x;
    m_StartY = y;
    m_Radius1 = radius1;
    m_Radius2 = radius2;
}

QJsonObject Ellipse::toJson() const
{
    QJsonObject data;
    data["x"] = m_StartX;
    data["y"] = m_StartY;
    data["radius"] = m_Radius1;
    data["radius"] = m_Radius2;

    return {
        {"type","ellipse"},
        {"data",data}
    };

}

bool Ellipse::fromJson(const QJsonObject &json)
{
    QJsonObject data = json["data"].toObject();
    if(!data.isEmpty()){
        double x = data["x"].toDouble();
        double y = data["y"].toDouble();
        double radius1 = data["radius"].toDouble();
        double radius2 = data["radius"].toDouble();
        this->setRadius1(radius1);
        this->setRadius2(radius2);
        this->setStartX(x);
        this->setStartY(y);
        return true;
    }
    return false;

}
