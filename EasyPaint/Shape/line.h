#ifndef LINE_H
#define LINE_H

#include <shape.h>
#include <QColor>

class Line : public Shape
{
public:
    Line();
    Line(const double&x1,const double&y1,const double&x2,const double&y2);
    ~Line() = default;

public:
    inline void setStartX(const double&x){m_StartX = x;}
    inline void setStartY(const double&y){m_StartY = y;}
    inline void set_EndX(const double&x){m_EndX = x;}
    inline void set_EndY(const double&y){m_EndY = y;}
    inline const double& getStartX(){return m_StartX;}
    inline const double& getStartY(){return m_StartY;}
    inline const double& getEndX(){return m_EndX;}
    inline const double& getEndY(){return m_EndY;}

private:
    double m_StartX;
    double m_StartY;
    double m_EndX;
    double m_EndY;
    QColor color;

    // Shape interface
public:
    QJsonObject toJson() const override;
    bool fromJson(const QJsonObject &json) override;
};

#endif // LINE_H
