#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include "Particle.h"

// ToDo: The L&H fields aren't set by the programs. Instead they are set by the controller.
// Use two separate structures. One like this for the controller, and one with just intended
// for the programs to use.
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
    AllDay = 5,
    OneHrOverride = 6
};

enum Schedules {
    Zone1 = 1,
    Zone2 = 2,
    AwaySched = 3
};

class Program {
public:
    virtual void getCurrentSetpoint(time_t now, struct SetPoint& setpoint) = 0;
};
class ViewableProgram : public Program {
public:
    virtual void getSchedule(int day, ProgramPoints* programPoints) = 0;
};
class ProgrammableProg : public ViewableProgram {
protected:
    ProgramPoints temperatureProgram[7];
public:
    void getCurrentSetpoint(time_t now, struct SetPoint& setpoint);
    void getSchedule(int day, ProgramPoints* programPoints);
    void updateTemp(int day, int progIndex, float temperature);
    void updateTime(int day, int progIndex, long setTime);
};
class SetableProg : public Program {
protected:
    float setPoint;
public:
    virtual void SetSetpoint(float temperature) = 0;
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
};
class OnProg : public Program {
public:
    void getCurrentSetpoint(time_t now, struct SetPoint& setpoint);
};
class TemporaryProg : public SetableProg {
public:
    void SetSetpoint(float temperature);
    void getCurrentSetpoint(time_t now, struct SetPoint& setpoint);
};

class Programmer {
    Program* currentProgram[2];
    OffProg* offProgLocal;
    OnProg* onProgLocal;
    ScheduleProg* z1SchedLocal;
    ScheduleProg* z2SchedLocal;
    AwayProg* awayProg;

    Program* previousProg[2];
    TemporaryProg* tempProg[2];
public:
    // zone: 1, 2. programId: 1: Off,
    Programmer();
    void selectProgram(int zone, ProgramIds programId);
    void getCurrentSetpoint(int zone, time_t now, struct SetPoint& setpoint);

    void getSchedule(int schedule, int day, ProgramPoints* programPoints);
    void updateTemp(int schedule, int day, int progIndex, float temperature);
    void updateTime(int schedule, int day, int progIndex, long setTime);

    void setOverride(int zone, float temp);
    void resetOverride(int zone);
};

#endif
