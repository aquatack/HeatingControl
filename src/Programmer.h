#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include "Particle.h"

struct SetPoint {
    float intended;
    float intendedL;
    float intendedH;
};

void getCurrentSetpoint(time_t now, struct SetPoint& setpoint);
void getSchedule(int schedule, int zone, int day, float* setPoints);
void initialiseScheduledTemps();

#endif
