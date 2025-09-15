#include "triangle.h"


Triangle::Triangle():
    m_Height(0),
    m_Width(0),
    m_StartX(0),
    m_StartY(0)
{

}

Triangle::Triangle(const double &x, const double &y, const double &w, const double &h):
    m_Height(h),
    m_Width(w),
    m_StartX(x),
    m_StartY(y)
{

}

QJsonObject Triangle::toJson() const
{
    QJsonObject data;
    data["x"] = m_StartX;
    data["y"] = m_StartY;
    data["width"] = m_Width;
    data["height"] = m_Height;

    return {
        {"type","triangle"},
        {"data",data}
    };
}

bool Triangle::fromJson(const QJsonObject &json)
{
    QJsonObject data = json["data"].toObject();
    if(!data.isEmpty()){
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
