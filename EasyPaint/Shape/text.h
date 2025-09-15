#ifndef TEXT_H
#define TEXT_H

#include <shape.h>
#include <QColor>

class Text : public Shape
{
public:
    Text();
    Text(const double&x,const double&y,const QString&str);
    ~Text() = default;

public:
    inline void setStartX(const double &x){m_StartX = x;}
    inline void setStartY(const double &y){m_StartY = y;}
    inline void setContext(const QString&str){m_Content = str;}
    inline const double& getStartX(){return m_StartX;}
    inline const double& getStartY(){return m_StartY;}
    inline const QString& getContent(){return m_Content;}
private:
    double m_StartX;
    double m_StartY;
    QString m_Content;
    int m_FontSize;
    int m_FontWidget;
    QString m_FontFamily;
    QColor color;

    // Shape interface
public:
    QJsonObject toJson() const override;
    bool fromJson(const QJsonObject &json) override;
};

#endif // TEXT_H
