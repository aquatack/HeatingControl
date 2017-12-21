#include "Programmer.h"


// ToDo: make static.
Programmer::Programmer()
{
    offProgLocal = new OffProg();
    onProgLocal = new OnProg();
    z1SchedLocal = new ScheduleProg();
    z2SchedLocal = new ScheduleProg();
    awayProg = new AwayProg();
}

void Programmer::selectProgram(int zone, ProgramIds programId)
{
    Serial.printf("selecting a program: zone %d, progId %d\n", zone, programId);
    if(programId == ProgramIds::Off)
    {
        Serial.printf("off prog selected.\n" );
        currentProgram[--zone] = offProgLocal;
    }
    else if(programId == ProgramIds::On)
    {
        Serial.printf("On prog selected.\n" );
        currentProgram[--zone] = onProgLocal;
    }
    else if(programId == ProgramIds::Schedule)
    {
        if(zone == 1)
        {
            Serial.printf("z1 schedule selected.\n" );
            currentProgram[--zone]= z1SchedLocal;
        } else if (zone  == 2){
            Serial.printf("z2 schedule selected.\n" );
            currentProgram[--zone]= z2SchedLocal;
        }
    }
    else if(programId == ProgramIds::Away)
    {
        Serial.printf("Away prog selected.\n" );
        currentProgram[--zone]= awayProg;
    }
}

void Programmer::getCurrentSetpoint(int zone, time_t now, struct SetPoint& setpoint)
{
    Serial.printf("Getting current set point for zone %d\n", zone);
    return currentProgram[--zone]->getCurrentSetpoint(now, setpoint);
}

void Programmer::getSchedule(int schedule, int day, float* setPoints)
{
    if(schedule == 1) {
        z1SchedLocal->getSchedule(day, setPoints);
    } else if(schedule == 2) {
        Serial.printf("getting zone 2 schedule.\n" );
        z2SchedLocal->getSchedule(day, setPoints);
    } else if(schedule == 3) {
        awayProg->getSchedule(day, setPoints);
    }
}
void Programmer::updateTemp(int schedule, int day, int hour, float temperature)
{
    if(schedule == 1) {
        z1SchedLocal->updateTemp(day, hour, temperature);
    } else if(schedule == 2) {
        z2SchedLocal->updateTemp(day, hour, temperature);
    } else if(schedule == 3) {
        awayProg->updateTemp(day, hour, temperature);
    }
}
// ========================================

ScheduleProg::ScheduleProg()
{
    Serial.printf("ScheduleProg constructor.\n" );
    for(int j=0;j<7;j++)
    {
        for(int i = 0; i<1; i++)
        {
            ScheduledTemps[j][i] = 21.0f;
        }
        for(int i = 1; i<4; i++)
        {
            ScheduledTemps[j][i] = 19.0f;
        }
        for(int i = 4; i<9; i++)
        {
            ScheduledTemps[j][i] = 21.0f;
        }
        for(int i = 9; i<17; i++)
        {
            ScheduledTemps[j][i] = 17.0f;
        }
        for(int i = 17; i<22; i++)
        {
            ScheduledTemps[j][i] = 22.0f;
        }
        for(int i = 22; i<24; i++)
        {
            ScheduledTemps[j][i] = 21.0f;
        }
    }
}

AwayProg::AwayProg()
{
    for(int j=0;j<7;j++)
    {
        for(int i = 0; i<24; i++)
        {
            ScheduledTemps[j][i] = 15.0f;
        }
    }
}

void ProgrammableProg::getSchedule(int day, float* setPoints)
{
    --day;
    for(int i = 0; i<24; i++)
    {
        setPoints[i] = ScheduledTemps[day][i];
    }
}

void ProgrammableProg::updateTemp(int day, int hour, float temperature)
{
    --day;
    Serial.printf("updateTemp Schedule: day: %d, hour: %d, temp: %3.2f\n",
         day,  hour,  temperature);
    ScheduledTemps[day][hour] = temperature;
}

void ProgrammableProg::getCurrentSetpoint(time_t now, struct SetPoint& setpoint)
{
    Serial.printf("In ScheduleProg::getCurrentSetpoint.\n");
    int weekday = Time.weekday(now) - 2;
    if(weekday < 0) { // adjust for Sunday
        weekday = 6;
    }
    int hour = Time.hour(now);
    Serial.printf("requested time: %d, %d: value: %3.2f\n",
        weekday, hour, ScheduledTemps[weekday][hour]);
    setpoint.intended = ScheduledTemps[weekday][hour];
}

void OffProg::getCurrentSetpoint(time_t now, struct SetPoint& setpoint)  {
    Serial.printf("In OffProg::getCurrentSetpoint.\n");
    setpoint.intended = 0.0f;
}
void OffProg::getSchedule(int day, float* setPoints)  {
    for(int i = 0; i<24; i++)
    {
        setPoints[i] = 0.0f;
    }
}
void OffProg::updateTemp(int day, int hour, float temperature)  {
    return; // do nothing.
}

void OnProg::getCurrentSetpoint(time_t now, struct SetPoint& setpoint)  {
    Serial.printf("In OnProg::getCurrentSetpoint.\n");
    setpoint.intended = 999.0f;
}
void OnProg::getSchedule(int day, float* setPoints)  {
    for(int i = 0; i<24; i++)
    {
        setPoints[i] = 999.0f;
    }
}
void OnProg::updateTemp(int day, int hour, float temperature)  {
    return; // do nothing.
}
