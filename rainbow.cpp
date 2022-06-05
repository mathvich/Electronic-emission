#include "rainbow.h"

rainbow::rainbow(QWidget *parent) : QWidget(parent)
{

}

void rainbow::paintEvent(QPaintEvent *event)
{
    QPainter painter;

    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int h = this->height();
    int w = this->width();

    for (int i=0; i<w; ++i)
    {
        if (i <= w/2)
        {            
            int G = 255.0*(float(i)/(w/2.0));
            int R = 255 - G;

            painter.setPen(QPen(QColor(R, G, 0)));
            painter.drawLine(QPoint(i, 0), QPoint(i, h));            
        }
        else
        {
            int B = 255.0*(float(i - w/2.0)/(w/2.0));
            int G = 255 - B;

            painter.setPen(QPen(QColor(0, G, B)));
            painter.drawLine(QPoint(i, 0), QPoint(i, h));
        }
    }

    painter.end();
}
