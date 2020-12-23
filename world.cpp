#include "world.h"

#include <stdlib.h>
#include <math.h>
#include <QtOpenGL>
//#include <QDebug>

world::world(int _Nparticles, int _Nphotons, float _SPDparticles, float _BOXsize):Nparticles(_Nparticles),
    Nphotons(_Nphotons),
    SPDparticles(_SPDparticles),
    BOXsize(_BOXsize)
{
    //memory allocation
    a = new particle[Nparticles];
    photons = new particle[Nphotons];
    color = 0;

    //initial positions and speeds of photons
    for (int i=0; i < Nphotons; ++i)
    {
        photons[i].initPhoton(false);
    }

    //initial positions and speeds of particles
    Rstep = 1.75 * BOXsize / sqrt(Nparticles);
    initParticlePositions();

    aDraw = a;

    calcThread = new processingThread(Nparticles, a, Nphotons, photons);

    TimerMeasure = new QTimer();
    TimerMeasure->setInterval(250);
    connect(TimerMeasure, SIGNAL(timeout()), this, SLOT(measureHandle()));
    TimerMeasure->start();

    //temperatureMeasure();
}

void world::initParticlePositions()
{
    float x = -BOXsize + Rstep;
    float y = -BOXsize + Rstep;
    float z = 0.0;

    for (int i = 0; i < Nparticles; ++i)
    {
        a[i].R = point(x, y/*, z*/);
        x += Rstep;
        if (x >= BOXsize - Rstep)
        {
            x = -BOXsize + Rstep;
            y += Rstep;
        }
        if (y >= BOXsize - Rstep)
        {
            x = -BOXsize + Rstep;
            y = -BOXsize + Rstep;
            z += Rstep;
        }

        // speeds
        a[i].dR = point(SPDparticles*(-1.0 + 2.0*float(rand()) / RAND_MAX),
                        SPDparticles*(-1.0 + 2.0*float(rand()) / RAND_MAX)/*,
                        0.0*SPDparticles*(-1.0 + 2.0*float(rand()) / RAND_MAX)*/);
        //a[i].dR = 0.0;
    }
}

void world::measureHandle()
{
    this->temperatureMeasure();
    this->externalLoop();
}

float world::temperatureMeasure()
{
    float Tavg = 0.0;
    int N = 1;
    for (int i=0; i<Nparticles; ++i)
    {
        if ((a[i].R.x > -1.0) && (a[i].R.x < 1.0) && (a[i].R.y > -1.0) && (a[i].R.y < 1.0))
        {
            float Ti = a[i].dR.sqrmodule();
            a[i].kineticEnergy = Ti;
            Tavg += Ti;
            ++N;
        }
    }
    Tavg /= float(N);
    calcThread->Tcurr = Tavg;
    emit newTemperature(Tavg);
    return Tavg;
}

void world::externalLoop()
{
    float y = -1.1;
    static float x = -BOXsize + Rstep;
    for (int i=0; i<Nparticles; ++i)
    {
        point R = aDraw[i].R;
        if ((R.x < -3.0) || (R.x > 3.0) || (R.y < -3.0) || (R.y > 6.0))
        {
            a[i].dR = point(0.0, 3.0*SPDparticles);
            a[i].R = point(x, y);
            x += Rstep;
            if (x > BOXsize - Rstep)
                x = -BOXsize + Rstep;
            y -= Rstep;
        }
    }

    for (int i=0; i<Nphotons; ++i)
    {
        point R = photons[i].R;
        if ((R.y < -1.0))
        {
            photons[i].initPhoton(false);
        }
    }
}

void world::drawParticles()
{
    glColor3f(1.0, 1.0, 1.0);
    if (aDraw)
        for (int i=0; i<Nparticles; ++i)
            aDraw[i].drawParticle();

    float R=0.0, G=0.0, B=0.0;
    if (color < 500)
    {
        R = 1.0 - color/500.0;
        G = color/500.0;
    }
    else
    {
        G = 1.0 - (color-500.0)/500.0;
        B = (color-500.0)/500.0;
    }
    glColor3f(R, G, B);
    if (photons)
        for (int i=0; i<Nphotons; ++i)
            photons[i].drawPhoton();
}

void world::setColor(int _color)
{
    color = _color;
}

void world::setIntensity(int _N)
{
    calcThread->setIntensity(_N);
}

void world::setFrequency(int _F)
{
    calcThread->setFrequency(0.5 + float(_F)/1000.0);
}

void world::setT(int _T)
{
    calcThread->setT(_T);
}

void world::setU(int _U)
{
    calcThread->setU(_U);
}

float world::getT()
{
    return Tcurr;
}

void world::startCalc()
{
    calcThread->start();
}

void world::set_dt(float _dt)
{
    calcThread->setdt(_dt);
}

void world::setRelaxation(bool _state)
{
    calcThread->relaxation = _state;
}

unsigned long world::getFrames()
{
    return calcThread->getFrames();
}
