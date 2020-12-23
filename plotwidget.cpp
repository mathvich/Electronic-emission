#include "plotwidget.h"
//#include <QPainter>

plotWidget::plotWidget()
{
    this->setFixedWidth(500);
    xCross = 0.0;
    yCross = 0.0;
}

/*
QSize plotWidget::minimumSizeHint()
{
    return QSize(600, 300);
}

QSize plotWidget::sizeHint()
{
    return QSize(700, 400);
}*/

void plotWidget::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << event->pos().x();
    /*
    if (event->buttons() & Qt::RightButton)
    {
        qDebug() << this->xAxis->pixelToCoord(event->pos().x());
    }
    */

    QCustomPlot::mousePressEvent(event);
}

void plotWidget::paintEvent(QPaintEvent *event)
{
    QCustomPlot::paintEvent(event);    
    if (1)
    {
        QPainter painter;
        painter.begin(this);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(QPen(QColor(255, 0, 0), 2));
            QString str = "Текущее состояние";
            //QString tmp = "";
            //str += "t=";
            //tmp.sprintf("%.2lf", mouseX);
            painter.drawText(QPoint(this->xAxis->coordToPixel(xCross-0.45), this->yAxis->coordToPixel(yCross+0.05)), str);
            float crossSize = 5;
            //painter.drawLine(xCross-crossSize, yCross, xCross+crossSize, yCross);
            painter.drawLine(this->xAxis->coordToPixel(xCross)-crossSize,
                             this->yAxis->coordToPixel(yCross),
                             this->xAxis->coordToPixel(xCross)+crossSize,
                             this->yAxis->coordToPixel(yCross));
            painter.drawLine(this->xAxis->coordToPixel(xCross),
                             this->yAxis->coordToPixel(yCross)-crossSize,
                             this->xAxis->coordToPixel(xCross),
                             this->yAxis->coordToPixel(yCross)+crossSize);
        painter.end();
    }    
}
