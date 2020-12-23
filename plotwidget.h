#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <qcustomplot.h>

class plotWidget : public QCustomPlot
{    
public:
    float xCross, yCross;
    plotWidget();
    //QSize minimumSizeHint();
    //QSize sizeHint();
    void mousePressEvent(QMouseEvent *event);

private:
    void paintEvent(QPaintEvent *event);
};

#endif // PLOTWIDGET_H
