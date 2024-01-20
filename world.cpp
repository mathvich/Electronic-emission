#include "world.h"

#include <stdlib.h>
#include <math.h>
#include <QtOpenGL>

world::world(int _Nparticles, int _Nphotons, float _SPDparticles, float _BOXsize):Nparticles(_Nparticles),
    Nphotons(_Nphotons),
    SPDparticles(_SPDparticles),
    BOXsize(_BOXsize)
{
    // arrays
    a = new particle[Nparticles];
    photons = new particle[Nphotons];
    color = 0;

    // initial positions and speeds of photons
    for (int i=0; i < Nphotons; ++i)
    {
        photons[i].initPhoton(false);
    }

    // initial positions and speeds of particles
    Rstep = 1.75 * BOXsize / sqrt(Nparticles);
    initParticlePositions();

    aDraw = a;

    // threads
    int threadsNumber = 2;// QThread::idealThreadCount();
    calculationThreads.resize(threadsNumber);

    // i*n/(t-1), (i+1)*n/(t-1)
    // n = 17
    // t = 3
    // i = 0; i < threadcount

    // i=0lo = 0 * 8; 0
    // i=0hi = 1 * 8; 8
    // i=1lo = 1 * 8; 8
    // i=1hi = 2 * 8; 16
    // i=2lo = 2 * 8; 16
    // i=2hi = 3 * 8; 24

    for (int i=0; i < threadsNumber; ++i)
    {
        const unsigned int maxIndex = Nparticles - 1;
        const unsigned int chunkSize = maxIndex / (threadsNumber - 1);

        const unsigned int indexLower = i * chunkSize;
        unsigned int indexUpper = (i + 1) * chunkSize;
        indexUpper = indexUpper > maxIndex ? maxIndex : indexUpper;

        calculationThreads[i] = new processingThread(i == 0 ? mainThread : auxThread,
                                                     indexLower, indexUpper,
                                                     Nparticles, a, Nphotons, photons);
    }
    calcThread = calculationThreads[0];

    // timers
    TimerMeasure = new QTimer();
    TimerMeasure->setInterval(250);
    connect(TimerMeasure, SIGNAL(timeout()), this, SLOT(measureHandle()));
    TimerMeasure->start();
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
        if ((a[i].R.x > -1.0) && (a[i].R.x < 1.0)
                && (a[i].R.y > -1.0) && (a[i].R.y < 1.0))
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
        G = color/500.0;
        R = 1.0 - G;

    }
    else
    {
        B = (color - 500.0)/500.0;
        G = 1.0 - B;
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
    // calcThread->start();
    for (int i=0; i<calculationThreads.size(); ++i)
        calculationThreads.at(i)->start();
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
