#ifndef PARTICLE_H
#define PARTICLE_H

#include "point.h"

class particle
{
public:
	point R, dR;
    bool enabled;
    float kineticEnergy;    
	particle();
	void drawParticle();
    void drawPhoton();
    void initPhoton(bool _enable);
};

#endif
