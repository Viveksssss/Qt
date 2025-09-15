#ifndef CURSOR_H
#define CURSOR_H

#include <shape.h>

class Cursor : public Shape
{
public:
    Cursor();

    // Shape interface
public:
    QJsonObject toJson() const override;
    bool fromJson(const QJsonObject &json) override;
};

#endif // CURSOR_H
