#include "point.h"

Point::Point(int x, int y) : _x(x), _y(y) {}

Point::Point() : Point(0., 0.) {}

double Point::x()
{
    return _x;
}

double Point::y()
{
    return _y;
}

double Point::width()
{
    return _x;
}

double Point::height()
{
    return _y;
}
