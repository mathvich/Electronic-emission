#include "processingthread.h"
#include <math.h>

processingThread::processingThread(int _Nparticles, particle *array, int _Nphotons, particle *photons, float _dt)
{
    //Nthreads = QThread::idealThreadCount();

    framesDone = 0;
    relaxation = true;

    T = 10.0;
    U = 0.0;

    dt = _dt;

    Nphotons = _Nphotons;
    p = photons;

    Nparticles = _Nparticles;
    a = array;
    b = new particle[Nparticles];
    arg = new particle[Nparticles];
    k1 = new particle[Nparticles];
    k2 = new particle[Nparticles];
    k3 = new particle[Nparticles];
    k4 = new particle[Nparticles];
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
    Intensity = _N;
}

void processingThread::setFrequency(float _F)
{
    Frequency = _F;
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

void processingThread::fKernel(particle *in, particle *out, int i)
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
    for (int j = 0; j < Nparticles; ++j)
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

void processingThread::sumKernel(particle *in1, particle *in2, particle *out, float dt, int i)
{
    //int i = threadIdx.x;
    //int i = threadIdx.x + blockIdx.x * blockDim.x;

    out[i].R = in1[i].R + in2[i].R*dt;
    out[i].dR = in1[i].dR + in2[i].dR*dt;
}

void processingThread::movePhotons(particle *_p, float _dt)
{
    for (int i=0; i<Nphotons; ++i)
        if (_p[i].enabled)
            _p[i].R = _p[i].R + _p[i].dR * _dt;
}

void processingThread::photonEnabler(int _N, float _dt)
{
    static float lastPhotonTime = 0.025;

    if (lastPhotonTime < 0.0)
    {
        int count = 0;
        for (int i=0; i<Nphotons; ++i)
            count += p[i].enabled;

        for (int i=0; i<Nphotons; ++i)
        {
            if (1)//count < _N)
                if (p[i].enabled == false)
                {
                    p[i].initPhoton(true);
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

void processingThread::run()
{
    // Euler's method
    /*
    for (int j=0; j<1000; ++j)
    {
        for (int i=0; i<Nparticles; ++i)
            fKernel(a, k1, i);
        for (int i = 0; i<Nparticles; ++i)
            sumKernel(a, k1, arg, dt, i);
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
        for (int i=0; i<Nparticles; ++i)
            fKernel(a, k1, i);
        for (int i = 0; i<Nparticles; ++i)
            sumKernel(a, k1, arg, dt / 2.0, i);
        for (int i = 0; i<Nparticles; ++i)
            fKernel(arg, k2, i);
        for (int i = 0; i<Nparticles; ++i)
            sumKernel(a, k2, arg, dt / 2.0, i);
        for (int i = 0; i<Nparticles; ++i)
            fKernel(arg, k3, i);
        for (int i = 0; i<Nparticles; ++i)
            sumKernel(a, k3, arg, dt / 1.0, i);
        for (int i = 0; i<Nparticles; ++i)
            fKernel(arg, k4, i);
        for (int i = 0; i<Nparticles; ++i)
            sumKernel(a, k1, b, dt / 6.0, i);
        for (int i = 0; i<Nparticles; ++i)
            sumKernel(b, k2, a, dt / 3.0, i);
        for (int i = 0; i<Nparticles; ++i)
            sumKernel(a, k3, b, dt / 3.0, i);
        for (int i = 0; i<Nparticles; ++i)
            sumKernel(b, k4, a, dt / 6.0, i);

        movePhotons(p, dt);
        photonEnabler(Intensity, dt);
        photonInteract();

        framesDone++;
    }
}

void processingThread::photonInteract()
{
    for (int i=0; i<Nphotons; ++i)
        if (p[i].enabled)
            for (int j=0; j<Nparticles; ++j)
                if ((a[j].R - p[i].R).moduleXY() < (0.15/2.0))
                {
                    a[j].dR = a[j].dR + p[i].dR*0.8*Frequency;
                    p[i].enabled = false;
                }
}

unsigned long processingThread::getFrames()
{
    unsigned long tmp = framesDone;
    framesDone = 0;
    return tmp;
}
