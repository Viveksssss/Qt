#include "line.h"


Line::Line():
    m_StartX(0),
    m_StartY(0),
    m_EndX(0),
    m_EndY(0)
{

}

Line::Line(const double &x1, const double &y1, const double &x2, const double &y2):
    m_StartX(x1),
    m_StartY(y1),
    m_EndX(x2),
    m_EndY(y2)
{

}

QJsonObject Line::toJson() const
{
    QJsonObject data;
    data["x1"] = m_StartX;
    data["y1"] = m_StartY;
    data["x2"] = m_EndX;
    data["y2"] = m_EndY;

    return {
        {"type","line"},
        {"data",data}
    };
}

bool Line::fromJson(const QJsonObject &json)
{
    QJsonObject data = json["data"].toObject();
    if(!data.isEmpty()){
        double x1 = data["x1"].toDouble();
        double y1 = data["y1"].toDouble();
        double x2 = data["x2"].toDouble();
        double y2 = data["y2"].toDouble();
        this->set_EndX(x2);
        this->set_EndY(y2);
        this->setStartX(x1);
        this->setStartY(y1);
        return true;
    }
    return false;
}
