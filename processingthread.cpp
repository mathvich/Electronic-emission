#include "processingthread.h"
#include <math.h>
#include <QDebug>

processingThread::processingThread(threadTypes threadType_, int indexLower_, int indexUpper_,
                                   int _particlesNumber, particle *_particles,
                                   int _photonsNumber,   particle *_photons,
                                   float _dt)
{
    threadType = threadType_;
    indexLower = indexLower_;
    indexUpper = indexUpper_;

    framesDone = 0;

    T = 10.;
    U = 0.;

    dt = _dt;

    photonsNumber = _photonsNumber;
    photons       = _photons;

    particlesNumber = _particlesNumber;
    particles       = _particles;

    particlesAux = new particle[particlesNumber];
    k1 = new particle[particlesNumber];
    k2 = new particle[particlesNumber];
    k3 = new particle[particlesNumber];
    k4 = new particle[particlesNumber];
}

double sqrDegree(double x);
double hexDegree(double x);

void processingThread::sumKernel(particle *in1, particle *in2, particle *out, float dt)
{
    for (int i = indexLower; i < indexUpper; ++i)
    {
        out[i].R  = in1[i].R  + in2[i].R  * dt;
        out[i].dR = in1[i].dR + in2[i].dR * dt;
    }
}

void processingThread::forceKernel(particle *in, particle *out)
{
    for (int i = indexLower; i < indexUpper; ++i)
    {
        const float e  = 1.35*1e-0;//1e-7;	// multiplier for (e/r)^n
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
            ttmp = hexDegree(in[i].R.x / width);
            out[i].dR.x += -16. * deepn * .1 * ttmp / in[i].R.x / sqrDegree(ttmp + 1.);

            ttmp = hexDegree(in[i].R.y / width);
            out[i].dR.y += -16. * deepn * .1 * ttmp / in[i].R.y / sqrDegree(ttmp + 1.);
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
            out[i].dR.x +=  W*exp(W*(-L - in[i].R.x));
            out[i].dR.y += -W*exp(W*(in[i].R.y - L));
            out[i].dR.y +=  W*exp(W*(-L - in[i].R.y));

            // Friction
            out[i].dR = out[i].dR + in[i].dR * (-5.);
        }

        /*
    // Cylindric
    float r = in[i].R.moduleXY();
    out[i].dR.x = -W*in[i].R.x / r*exp(W*(r - L));
    out[i].dR.y = -W*in[i].R.y / r*exp(W*(r - L));
    */

        // Temperature influence
        float delta = T - Tcurr;
        float deltaTrim = 3.;

        if (delta > deltaTrim)
            delta = deltaTrim;
        else if (delta < -deltaTrim)
            delta = -deltaTrim;

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
                float rm = r.module() / e;
                out[i].dR = out[i].dR + r * (er / (rm * rm * rm));
            }
        }
    }
}

void processingThread::doCalculation()
{
    if (0)
    {
        // Euler's calculation method
        forceKernel(particles, k1);
        sumKernel(particles, k1, particles, dt);
    }
    else
    {
        // Runge-Kutta 4 calculation method
        forceKernel(particles, k1);

        sumKernel(particles, k1, particlesAux, dt / 2.);
        forceKernel(particlesAux, k2);

        sumKernel(particles, k2, particlesAux, dt / 2.);
        forceKernel(particlesAux, k3);

        sumKernel(particles, k3, particlesAux, dt / 1.);
        forceKernel(particlesAux, k4);

        sumKernel(particles, k1, particlesAux, dt / 6.);
        sumKernel(particlesAux, k2, particles, dt / 3.);
        sumKernel(particles, k3, particlesAux, dt / 3.);
        sumKernel(particlesAux, k4, particles, dt / 6.);
    }

}

void processingThread::run()
{
    for (unsigned int count=0; count<3000; count++)
    {
        doCalculation();
    }

    relaxation = false;

    while (this->isRunning())
    {
        doCalculation();

        if (threadType == mainThread)
        {
            movePhotons(photons, dt);
            photonEnabler(intensity, dt);
            photonInteract();
            framesDone++;
        }
        // to do: sync here via thread barrier
    }
}

void processingThread::movePhotons(particle *_photons, float _dt)
{
    for (int i=0; i<photonsNumber; ++i)
        if (_photons[i].enabled)
            _photons[i].R = _photons[i].R + _photons[i].dR * _dt;
}

void processingThread::photonEnabler(int _N, float _dt)
{
    static float lastPhotonTime = .025;

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
                    lastPhotonTime = .25 / float(_N + 1.);
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

void processingThread::setdt(float _dt)
{
    dt = _dt;
}

void processingThread::setT(int _T)
{
    T = _T * 42. / 1000.;
}

void processingThread::setU(int _U)
{
    U = _U;
}

void processingThread::setIntensity(int _intensity)
{
    intensity = _intensity;
}

void processingThread::setFrequency(float _frequency)
{
    frequency = _frequency;
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
    return x * x;
}
