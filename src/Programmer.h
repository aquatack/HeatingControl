#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include "Particle.h"

struct SetPoint {
    float intended;
    float intendedL;
    float intendedH;
};
class Programmer {
    float ScheduledTemps[7][24];
public:
    void getCurrentSetpoint(time_t now, int schedule, int zone, struct SetPoint& setpoint);
    void getSchedule(int schedule, int zone, int day, float* setPoints);
    void initialiseScheduledTemps();
    void updateTemp(int schedule, int zone, int day, int hour, float temperature);
};
#endif
