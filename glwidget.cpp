#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "glwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

void drawSquarePolygon(float x, float y, float L)
{
    float r = L/15.0;

    glBegin(GL_POLYGON);
        glVertex3f(x+L, y+L-r, -0.01);
        glVertex3f(x+L-r, y+L, -0.01);
        glVertex3f(x-L+r, y+L, -0.01);
        glVertex3f(x-L, y+L-r, -0.01);
        glVertex3f(x-L, y-L+r, -0.01);
        glVertex3f(x-L+r, y-L, -0.01);
        glVertex3f(x+L-r, y-L, -0.01);
        glVertex3f(x+L, y-L+r, -0.01);
    glEnd();
    /*
    glBegin(GL_POLYGON);
        glVertex3f(x-L, y-L, -0.01);
        glVertex3f(x+L, y-L, -0.01);
        glVertex3f(x+L, y+L, -0.01);
        glVertex3f(x-L, y+L, -0.01);
    glEnd();
    */
}

void glWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
        glEnable(GL_DEPTH_TEST);

        glRotatef(Angles.y(), 1.0, 0.0, 0.0);
        glRotatef(Angles.x(), 0.0, 0.0, 1.0);

        glScalef(Scale, Scale, Scale);
        glTranslatef(PCamera.x, PCamera.y, 0.0/*PCamera.z*/);

        World->drawParticles();

        //Cathode        
        glColor3f(0.4, 0.4, 0.4);
        drawSquarePolygon(0.0, 0.0, 1.0);

        //Anode
        glColor3f(0.4, 0.4, 0.4);
        drawSquarePolygon(0.0, 9.0, 3.0);

        glColor3f(1.0, 1.0, 1.0);
        renderText(-2.0, -0.5, 0.0, "Cathode", QFont("Arial", 15));
        renderText(-2.0, 5.0, 0.0, "Anode", QFont("Arial", 15));
    glPopMatrix();

}

glWidget::glWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    //setFixedSize(500, 500);
    Angles = QPoint(-90, 0);
    PCamera = point(0.0, 0.0/*, 0.0*/)*(-1.0);
    Scale = 1.4;
    Tpercent = 0.0;
}

void glWidget::setWorld(world *ptr)
{
    World = ptr;
}

glWidget::~glWidget(){}

/*QSize glWidget::sizeHint() const
{
    return QSize(800, 800);
}*/

void glWidget::setRotation(QPoint _Angles)
{
    if (_Angles != Angles) {
        Angles = _Angles;
        emit RotationChanged(_Angles);
        updateGL();
    }
}

void glWidget::setPosition(point _PCamera)
{
    if (_PCamera != PCamera) {
        PCamera = _PCamera;
        emit PositionChanged(_PCamera);
        updateGL();
    }
}

void glWidget::setScale(float _Scale)
{
    if (_Scale != Scale) {
        Scale = _Scale;
        emit ScaleChanged(_Scale);
        updateGL();
    }
}

void glWidget::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    //static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void glWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 9.0, -5.0, 5.0, -10.0, 10.0);
    //glOrtho(-10.0, 9.10, -10.0, 10.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

//void glWidget::keyPressEvent(QKeyEvent *event)
//{

//if (Qt::Key_Escape & event->key())
//{
//    this->close();
//        //shift view point
//        const float Pi = 3.14159265;
//        float alpha = (-Angles.x())/180.0*Pi;
//        alpha -= 90.0 / 180*Pi;
//        double step = 20;
//        setPosition(PCamera + 0.15*point(cos(alpha)*step, sin(alpha)*step));
//    }
//}

void glWidget::mousePressEvent(QMouseEvent *event)
{
    //set reference point
    PMouseZero = event->pos();
}

void glWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint MouseDelta = event->pos() - PMouseZero;

    //Mouse inversion
    //MouseDelta.setY(MouseDelta.y()*(-1));

    //Lock Axis
    //MouseDelta.setX(0);

    if (event->buttons() & Qt::RightButton)
    {
        //shift view point
        const float Pi = 3.14159265;
        float alpha = (-Angles.x())/180.0*Pi;
        setPosition(PCamera + point(cos(alpha)*MouseDelta.x(),
                                    sin(alpha)*MouseDelta.x())*(0.015/Scale));
        alpha -= 90.0 / 180*Pi;
        setPosition(PCamera + point(cos(alpha)*MouseDelta.y(),
                                    sin(alpha)*MouseDelta.y())*(0.015/Scale));
    }
    else if (event->buttons() & Qt::LeftButton)
    {
        //rotate across view point
        setRotation(Angles + 1 * MouseDelta);
    }
    else if (event->buttons() & Qt::MiddleButton){
        setScale(Scale*exp(-MouseDelta.y()*log(1.01)));
    }
    PMouseZero = event->pos();
}
