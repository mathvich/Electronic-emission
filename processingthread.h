#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#include <QThread>
#include "particle.h"

class world;

class processingThread : public QThread
{
    Q_OBJECT

private:
    float dt;
    float T, U;
    int intensity;
    float frequency;
    unsigned long framesDone;
    particle *particles, *particlesAux, *k1, *k2, *k3, *k4;
    particle *photons;

public slots:
    void setdt(float _dt);
    unsigned long getFrames();
    void setT(int _T);
    void setU(int _U);
    void setIntensity(int _N);
    void setFrequency(float _F);

public:
    bool relaxation;
    void run();
    float Tcurr;
    int particlesNumber;
    int photonsNumber;

    processingThread(int _Nparticles, particle *array, int _Nphotons, particle *photons, float _dt=1e-4);

    void movePhotons(particle *_p, float _dt);
    void photonEnabler(int _N, float _dt);
    void photonInteract();
    void setArray(particle *_a);
    void forceKernel(particle *in, particle *out, int i);
    void sumKernel(particle *in1, particle *in2, particle *out, float dt, int i);
};

#endif // PROCESSINGTHREAD_H
