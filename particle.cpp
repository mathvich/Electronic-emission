#include "particle.h"
#include <QtOpenGL>

particle::particle()
{
    kineticEnergy = 0.0;
    enabled = false;    
}

void particle::drawParticle()
{
	glPushMatrix();
        glTranslatef(R.x, R.y, 0.0/*R.z*/);
        //glutSolidSphere(0.015, 8, 8);

        const float pSize = 0.045;
        //glColor3f(1.0, 1.0, 1.0);
        /*
        glBegin(GL_POLYGON);
            glVertex2f(-pSize, -pSize);
            glVertex2f(pSize, -pSize);
            glVertex2f(pSize, pSize);
            glVertex2f(-pSize, pSize);
        glEnd();
        */

        float a = 0.0;
        glBegin(GL_POLYGON);
            while (a < 2.0*3.14159265)
            {
                glVertex2f(pSize*cos(a), pSize*sin(a));
                a += 0.75;
            }
        glEnd();

	glPopMatrix();
}

void particle::drawPhoton()
{
    if (enabled)
    {
        glPushMatrix();
            glTranslatef(R.x, R.y, 0.0/*R.z*/);
            //glColor3f(0.0, 1.0, 0.0);
            const float pSize = 0.15;
            glBegin(GL_POLYGON);
                glVertex2f(-pSize, -pSize);
                glVertex2f(pSize, -pSize);
                glVertex2f(pSize, pSize);
                glVertex2f(-pSize, pSize);
            glEnd();
        glPopMatrix();
    }
}

void particle::initPhoton(bool _enable)
{
    //float alpha = 0.9*3.14159/4.0;
    //R = point(2.5, 3.0);
    float alpha = 0.9*3.14159/8.0;
    R = point(2.5, 1.70);

    //alpha += 0.2*(-1.0 + 2.0*float(rand()) / RAND_MAX);
    alpha += 0.15*(-1.0 + 2.0*float(rand()) / RAND_MAX);
    dR = point(-cos(alpha), -sin(alpha)) * 20.0;
    enabled = _enable;
}
