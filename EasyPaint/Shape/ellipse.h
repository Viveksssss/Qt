#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <shape.h>
#include <QColor>

class Ellipse : public Shape
{
public:
    Ellipse();
    Ellipse(const double&x,const double&y,const double&radius1,const double&radius2);
    ~Ellipse() = default;

public:
    inline void setStartX(const double&x){m_StartX = x;}
    inline void setStartY(const double&y){m_StartY = y;}
    inline void setRadius1(const double&r){m_Radius1 = r;}
    inline void setRadius2(const double&r){m_Radius2 = r;}
    inline const double& getStartX(){return m_StartX;}
    inline const double& getStartY(){return m_StartY;}
    inline const double& getRadius1(){return m_Radius1;}
    inline const double& getRadius2(){return m_Radius2;}

private:
    double m_StartX;
    double m_StartY;
    double m_Radius1;
    double m_Radius2;
    QColor m_Color;


    // Shape interface
public:
    QJsonObject toJson() const override;
    bool fromJson(const QJsonObject &json) override;
};

#endif // ELLIPSE_H
