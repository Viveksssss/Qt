#include "text.h"


Text::Text():
    m_StartX(0),
    m_StartY(0),
    m_Content("")
{

}

Text::Text(const double &x, const double &y, const QString &str):
    m_StartX(x),
    m_StartY(y),
    m_Content(str)
{

}

QJsonObject Text::toJson() const
{
    QJsonObject data;
    data["x"] = m_StartX;
    data["y"] = m_StartY;
    data["content"] = m_Content;

    return {
        {"type","text"},
        {"data",data}
    };
}

bool Text::fromJson(const QJsonObject &json)
{
    QJsonObject data = json["data"].toObject();
    if(!data.isEmpty()){
        double x = data["x"].toDouble();
        double y = data["y"].toDouble();
        QString str = data["content"].toString();
        this->setContext(str);
        this->setStartX(x);
        this->setStartY(y);
        return true;
    }
    return false;
}
