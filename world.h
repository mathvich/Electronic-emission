#ifndef WORLD_H
#define WORLD_H

#include <QWidget>
#include <QThread>
#include <QTimer>
#include "particle.h"
#include "processingthread.h"

class world : public QWidget
{
    Q_OBJECT

    int Nthreads;
    float SPDparticles;
    float BOXsize;
    float Rstep;
    float Tcurr;
    processingThread *calcThread;

    QTimer *TimerMeasure;

    particle *a;
    particle *photons;
    int color;

public:
    int Nparticles;
    int Nphotons;
    particle *aDraw;

    world(int _Nparticles, int _Nphotons, float _SPDparticles, float _BOXsize);

    void initParticlePositions();
    void drawParticles();
    void externalLoop();
    float temperatureMeasure();


signals:
    void newTemperature(float value);

public slots:
    void setColor(int _color);
    void setFrequency(int _F);
    void setRelaxation(bool _state);
    void setT(int _T);
    void setIntensity(int _N);
    float getT();
    void setU(int _U);
    void startCalc();
    void set_dt(float _dt);
    unsigned long getFrames();
    void measureHandle();
};

#endif // WORLD_H
