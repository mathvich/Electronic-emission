#include "point.h"

#include <QtOpenGL>

void glVertex3p(point _P)
{
    glVertex2f(_P.x, _P.y/*, _P.z*/);
}

float point::module()
{
    return sqrt(x*x + y*y/* + z*z*/);
}
float point::moduleXY()
{
    return sqrt(x*x + y*y);
}
float point::sqrmodule()
{
    return (x*x + y*y);
}

point::point(const double &_x, const double &_y/*, double _z*/):x(_x), y(_y)/*, z(_z), enabled(false)*/{}

point point::operator+(const point &b)
{
    return point(x + b.x, y + b.y/*, z + b.z*/);
}

point point::operator-(const point &b)
{
    return point(x - b.x, y - b.y/*, z - b.z*/);
}

point point::operator*(const double &c)
{
    return point(c*x, c*y/*, c*z*/);
}

bool point::operator!=( const point& b)
{
    if ((x != b.x) || (y != b.y) /*|| (z != b.z)*/)
        return true;
    else
        return false;
}
