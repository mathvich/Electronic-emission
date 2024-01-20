#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#include <QThread>
#include <vector>
#include "particle.h"

enum threadTypes
{
    mainThread,
    auxThread,
    unknownThread
};

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

    int indexLower = 0;
    int indexUpper = 0;

public slots:
    void setdt(float _dt);
    unsigned long getFrames();
    void setT(int _T);
    void setU(int _U);
    void setIntensity(int _intensity);
    void setFrequency(float _frequency);

public:
    void run();

    threadTypes threadType = unknownThread;

    bool relaxation = true;
    float Tcurr;
    int particlesNumber;
    int photonsNumber;

    void doCalculation();

    processingThread(threadTypes threadType_, int indexLower_, int indexUpper_,
                     int _particlesNumber, particle *_particles,
                     int _photonsNumber, particle *_photons,
                     float _dt=1e-4);

    void movePhotons(particle *_photons, float _dt);
    void photonEnabler(int _N, float _dt);
    void photonInteract();

    void forceKernel(particle *in, particle *out);
    void sumKernel(particle *in1, particle *in2, particle *out, float dt);
};

#endif // PROCESSINGTHREAD_H
