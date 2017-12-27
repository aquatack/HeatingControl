#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include "Particle.h"

struct SetPoint {
    float intended;
    float intendedL;
    float intendedH;
};

// Holds the temp programms. Array of 7 allows 7 programmable times.
struct ProgramPoints {
    long startTime[7]; // in seconds
    float targetTemp[7];
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
    virtual void getCurrentSetpoint(time_t now, struct SetPoint& setpoint) = 0;
    virtual void getSchedule(int day, ProgramPoints* programPoints) = 0;
};

class ProgrammableProg : public Program {
protected:
    ProgramPoints temperatureProgram[7];
public:
    void getCurrentSetpoint(time_t now, struct SetPoint& setpoint);
    void getSchedule(int day, ProgramPoints* programPoints);
    void updateTemp(int day, int progIndex, float temperature);
    void updateTime(int day, int progIndex, long setTime);
};

class ScheduleProg : public ProgrammableProg {
public:
    ScheduleProg();
};

class AwayProg : public ProgrammableProg {
public:
    AwayProg();
};

class OffProg : public Program {
public:
    void getCurrentSetpoint(time_t now, struct SetPoint& setpoint);
    void getSchedule(int day, ProgramPoints* programPoints);
};

class OnProg : public Program {
public:
    void getCurrentSetpoint(time_t now, struct SetPoint& setpoint);
    void getSchedule(int day, ProgramPoints* programPoints);
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

    void getSchedule(int schedule, int day, ProgramPoints* programPoints);
    void updateTemp(int schedule, int day, int progIndex, float temperature);
    void updateTime(int schedule, int day, int progIndex, long setTime);
};

#endif
