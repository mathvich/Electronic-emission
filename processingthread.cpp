#include "processingthread.h"
#include <math.h>

processingThread::processingThread(int _particlesNumber, particle *_particles, int _photonsNumber, particle *_photons, float _dt)
{
    int threadsNumber = QThread::idealThreadCount();
    calculationThreads.resize(threadsNumber);
    for (int i=0; i < threadsNumber; ++i)
        calculationThreads[i] = new QThread;

    framesDone = 0;
    relaxation = true;

    T = 10.0;
    U = 0.0;

    dt = _dt;

    photonsNumber = _photonsNumber;
    photons = _photons;

    particlesNumber = _particlesNumber;
    particles = _particles;
    particlesAux = new particle[particlesNumber];
    k1 = new particle[particlesNumber];
    k2 = new particle[particlesNumber];
    k3 = new particle[particlesNumber];
    k4 = new particle[particlesNumber];
}

void processingThread::setdt(float _dt)
{
    dt = _dt;
}

void processingThread::setT(int _T)
{
    T = _T*42.0/1000.0;
}

void processingThread::setU(int _U)
{
    U = _U;
}

void processingThread::setIntensity(int _N)
{
    intensity = _N;
}

void processingThread::setFrequency(float _F)
{
    frequency = _F;
}

double hexDegree(double x)
{
    x *= x; // doub
    x *= x; // quad
    x *= x; // octa
    x *= x; // hexa

    return x;
}

double sqrDegree(double x)
{
    return x*x;
}

void processingThread::sumKernel(particle *in1, particle *in2, particle *out, float dt)
{
    //int i = threadIdx.x;

    for (int i = 0; i < particlesNumber; ++i)
    {
        out[i].R = in1[i].R + in2[i].R*dt;
        out[i].dR = in1[i].dR + in2[i].dR*dt;
    }
}

void processingThread::forceKernel(particle *in, particle *out)
{
    //int i = threadIdx.x;

    for (int i=0; i < particlesNumber; ++i)
    {
        const float e = 1.35*1e-0;//1e-7;	// multiplier for (e/r)^n
        const float er = 1.0;	// er*(e/r)^n

        const float deepn = 1.6*1e+3;
        const float width = 1.02;

        out[i].R = in[i].dR;

        /* ------------- Potentials --------------- */

        out[i].dR = point();

        float ttmp;
        // Metal hold Cathode
        if ((in[i].R.x > -1.0) && (in[i].R.x < 1.0)
                && (in[i].R.y > -1.0) && (in[i].R.y < 1.0))
        {
            ttmp = hexDegree( in[i].R.x / width );
            out[i].dR.x += -16.0*deepn*0.1*ttmp / in[i].R.x / sqrDegree(ttmp + 1.0);

            ttmp = hexDegree( in[i].R.y / width );
            out[i].dR.y += -16.0*deepn*0.1*ttmp / in[i].R.y / sqrDegree(ttmp + 1.0);
        }



        // Cathode-Anode electric field
        if ((in[i].R.y > 0.990) && (in[i].R.y < 5.0))
            out[i].dR.y += U;

        if ((in[i].R.y > 0.0) && (in[i].R.y < 1.0)
                && ((in[i].R.x < -1.0) || (in[i].R.x > 1.0)))
            out[i].dR.y += U*(in[i].R.y);

        if (relaxation)
        {
            const float W = 50.0;
            const float L = 0.98;
            // Dual infinite wall
            out[i].dR.x += -W*exp(W*(in[i].R.x - L));
            out[i].dR.x += W*exp(W*(-L - in[i].R.x));
            out[i].dR.y += -W*exp(W*(in[i].R.y - L));
            out[i].dR.y += W*exp(W*(-L - in[i].R.y));

            // Friction
            out[i].dR = out[i].dR + in[i].dR * (-5.0);
        }

        /*
    // Cylindric
    float r = in[i].R.moduleXY();
    out[i].dR.x = -W*in[i].R.x / r*exp(W*(r - L));
    out[i].dR.y = -W*in[i].R.y / r*exp(W*(r - L));
    */

        // Temperature influence
        float delta = T-Tcurr;
        if (delta > 3.0)
            delta = 3.0;
        else
            if (delta < -3.0)
                delta = -3.0;

        out[i].dR = out[i].dR + in[i].dR * (1.0*delta);


        // Particle interaction
        for (int j = 0; j < particlesNumber; ++j)
        {
            if (j != i)
            {
                point r = in[i].R - in[j].R;
                //float rm = r.module();
                //out[i].dR = out[i].dR + r*n*er*expf(-(n + 2.0)*logf(rm/e));

                //electrostatic
                float rm = r.module()/e;
                out[i].dR = out[i].dR + r*(er/(rm*rm*rm));
            }
        }
    }
}

void processingThread::run()
{
    // Euler's method
    /*
    for (int j=0; j<1000; ++j)
    {
        for (int i=0; i<particlesNumber; ++i)
            forceKernel(particles, k1, i);
        for (int i = 0; i<particlesNumber; ++i)
            sumKernel(particles, k1, particlesAux, dt, i);
        swap( particles, particlesAux );
    }*/

    unsigned int count = 0;

    while (this->isRunning())
    {
        if (count < 2000)
            count++;
        else
        {
            relaxation = false;
            count = 1001;
        }

        // Runge-Kutta 4
        forceKernel(particles, k1);

        sumKernel(particles, k1, particlesAux, dt / 2.0);
        forceKernel(particlesAux, k2);

        sumKernel(particles, k2, particlesAux, dt / 2.0);
        forceKernel(particlesAux, k3);

        sumKernel(particles, k3, particlesAux, dt / 1.0);
        forceKernel(particlesAux, k4);

        sumKernel(particles, k1, particlesAux, dt / 6.0);
        sumKernel(particlesAux, k2, particles, dt / 3.0);
        sumKernel(particles, k3, particlesAux, dt / 3.0);
        sumKernel(particlesAux, k4, particles, dt / 6.0);

        movePhotons(photons, dt);
        photonEnabler(intensity, dt);
        photonInteract();

        framesDone++;
    }
}

void processingThread::movePhotons(particle *_p, float _dt)
{
    for (int i=0; i<photonsNumber; ++i)
        if (_p[i].enabled)
            _p[i].R = _p[i].R + _p[i].dR * _dt;
}

void processingThread::photonEnabler(int _N, float _dt)
{
    static float lastPhotonTime = 0.025;

    if (lastPhotonTime < 0.0)
    {
        int count = 0;
        for (int i=0; i<photonsNumber; ++i)
            count += photons[i].enabled;

        for (int i=0; i<photonsNumber; ++i)
        {
            if (1)//count < _N)
                if (photons[i].enabled == false)
                {
                    photons[i].initPhoton(true);
                    lastPhotonTime = 0.25/float(_N+1.0);
                    break;
                    ++count;
                }
            //if (count >= _N)
            //    break;
        }
    }
    lastPhotonTime -= _dt;
}

void processingThread::photonInteract()
{
    for (int i=0; i<photonsNumber; ++i)
        if (photons[i].enabled)
            for (int j=0; j<particlesNumber; ++j)
                if ((particles[j].R - photons[i].R).moduleXY() < (0.15/2.0))
                {
                    particles[j].dR = particles[j].dR + photons[i].dR*0.8*frequency;
                    photons[i].enabled = false;
                }
}

unsigned long processingThread::getFrames()
{
    unsigned long tmp = framesDone;
    framesDone = 0;
    return tmp;
}
