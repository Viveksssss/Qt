#include "rectangle.h"

Rectangle::Rectangle():m_StartX(0)
,m_StartY(0)
,m_Height(0)
,m_Width(0)

{

}

QJsonObject Rectangle::toJson() const
{
    QJsonObject data;
    data["x"] = m_StartX;
    data["y"] = m_StartY;
    data["width"] = m_Width;
    data["height"] = m_Height;

    return {
        {"type","rectangle"},
        {"data",data}
    };
}

bool Rectangle::fromJson(const QJsonObject &json)
{
    QJsonObject data = json["data"].toObject();
    if(data.isEmpty()){
        double x = data["x"].toDouble();
        double y = data["y"].toDouble();
        double height = data["height"].toDouble();
        double width = data["width"].toDouble();
        this->setWidth(width);
        this->setHeight(height);
        this->setStartX(x);
        this->setStartY(y);
        return true;
    }
    return false;
}

Rectangle::Rectangle(const double &x, const double &y, const double &w, const double &h)
{
    this->m_StartX = x;
    this->m_StartY = y;
    this->m_Height = h;
    this->m_Width = w;
}

