#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "point.h"
#include "world.h"

class glWidget : public QGLWidget
{
    Q_OBJECT

public:
    float Tpercent;
    glWidget(QWidget *parent = 0);
    void setWorld(world* ptr);
    ~glWidget();
    //QSize sizeHint() const;

public slots:
    void setRotation(QPoint _Angles);
    void setPosition(point _PCamera);
    void setScale(float _Scale);

signals:
    void RotationChanged(QPoint _Angles);
    void PositionChanged(point _PCamera);
    void ScaleChanged(float _Scale);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    //void keyPressEvent(QKeyEvent *event);

private:
    world *World;

    float Scale;
    QPoint Angles; // .x is VIEW angle rOz, .y is VIEW angle rOx
    point PCamera; //current camera's position
    point PCameraZero; //camera's position at begin of mouse dragging
    QPoint PMouseZero; //cursor's position at begin of mouse dragging
};

#endif
