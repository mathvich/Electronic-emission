#ifndef POINT_H
#define POINT_H

class point
{
public:
    double x,y;//,z;
    //bool enabled;
    point(const double &_x=0, const double &_y=0);//, double _z=0);
    point operator+(const point &b);
    point operator-(const point &b);
    point operator*(const double &c);
    bool operator!=(const point& b);

    float module();
    float moduleXY();
    float sqrmodule();
};



void glVertex3p(point _P);

#endif
