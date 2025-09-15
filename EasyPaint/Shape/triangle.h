#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <shape.h>
#include <QColor>

class Triangle : public Shape
{
public:
    Triangle();
    Triangle(const double&x,const double&y,const double&w,const double&h);
    ~Triangle() = default;
public:
    inline void setStartX(const double&x){m_StartX = x;}
    inline void setStartY(const double&y){m_StartY = y;}
    inline void setWidth(const double&w){m_Width = w;}
    inline void setHeight(const double&h){m_Height = h;}
    inline const double& getStartX(){return m_StartX;}
    inline const double& getStartY(){return m_StartY;}
    inline const double& getWidth(){return m_Width;}
    inline const double& getHeight(){return m_Height;}

private:
    double m_StartX;
    double m_StartY;
    double m_Width;
    double m_Height;
    QColor m_Color;


    // Shape interface
public:
    QJsonObject toJson() const override;
    bool fromJson(const QJsonObject &json) override;
};

#endif // TRIANGLE_H
