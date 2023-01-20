#ifndef POINT_H_
#define POINT_H_

class Point {
public:
    Point(int x, int y);
    Point();

    double x();
    double y();

    double width();
    double height();
private:
    double _x;
    double _y;
};

typedef Point Dimension;

#endif