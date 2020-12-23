#include "rainbow.h"

rainbow::rainbow(QWidget *parent) : QWidget(parent)
{

}

void rainbow::paintEvent(QPaintEvent *event)
{
    QPainter painter;

    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //painter.fillRect(event->rect(), QBrush(QColor(54, 54, 35)));

    //comfort-sugar
    int h = this->height();
    int w = this->width();

    for (int i=0; i<w; ++i)
    {
        if (i<w/2)
        {
            painter.setPen(QPen(QColor(int(255.0*(1.0 - float(i)/float(w/2.0))),
                                       int(255.0*(float(i)/float(w/2.0))),
                                       0)));
            painter.drawLine(QPoint(i, 0), QPoint(i, h));
        }
        else
        {
            painter.setPen(QPen(QColor(0, int(255.0*(1.0 - float(i-w/2.0)/float(w/2.0))),
                                       int(255.0*(float(i-w/2.0)/float(w/2.0))))));
            painter.drawLine(QPoint(i, 0), QPoint(i, h));
        }
    }

    painter.end();
}
