#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include "Particle.h"

struct SetPoint {
    float intended;
    float intendedL;
    float intendedH;
};

enum ProgramIds {
    Off = 1,
    On = 2,
    Schedule = 3,
    Away = 4,
    AllDay = 5
};

class Program {
public:
    //Program();
    //~Program();
    virtual void getCurrentSetpoint(time_t now, struct SetPoint& setpoint) = 0;
    virtual void getSchedule(int day, float* setPoints) = 0;
    virtual void updateTemp(int day, int hour, float temperature) = 0;
};

class ProgrammableProg : public Program {
protected:
    float ScheduledTemps[7][24];
public:
    //ProgrammableProg() = 0;
    //~Schedule();
    void getCurrentSetpoint(time_t now, struct SetPoint& setpoint);
    void getSchedule(int day, float* setPoints);
    void updateTemp(int day, int hour, float temperature);
};

class ScheduleProg : public ProgrammableProg {
public:
    ScheduleProg();
    //~Schedule();
    //void getCurrentSetpoint(time_t now, struct SetPoint& setpoint);
    //void getSchedule(int day, float* setPoints);
    //void updateTemp(int day, int hour, float temperature);
};

class AwayProg : public ProgrammableProg {
public:
    AwayProg();
};

class OffProg : public Program {
public:
    void getCurrentSetpoint(time_t now, struct SetPoint& setpoint);
    void getSchedule(int day, float* setPoints);
    void updateTemp(int day, int hour, float temperature);
};

class OnProg : public Program {
public:
    void getCurrentSetpoint(time_t now, struct SetPoint& setpoint);
    void getSchedule(int day, float* setPoints);
    void updateTemp(int day, int hour, float temperature);
};

class Programmer {
    Program* currentProgram[2];
    OffProg* offProgLocal;
    OnProg* onProgLocal;
    ScheduleProg* z1SchedLocal;
    ScheduleProg* z2SchedLocal;
    AwayProg* awayProg;
public:
    // zone: 1, 2. programId: 1: Off,
    Programmer();
    void selectProgram(int zone, ProgramIds programId);
    void getCurrentSetpoint(int zone, time_t now, struct SetPoint& setpoint);
    //void getCurrentSchedule(int zone, int day, float* setPoints);

    void getSchedule(int schedule, int day, float* setPoints);
    void updateTemp(int schedule, int day, int hour, float temperature);
};

#endif
