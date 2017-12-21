#include "Programmer.h"


// ToDo: make static.
void Programmer::initialise()
{
    offProgLocal = new OffProg();
    onProgLocal = new OnProg();
    z1SchedLocal = new ScheduleProg();
    z2SchedLocal = new ScheduleProg();
}

void Programmer::selectProgram(int zone, ProgramIds programId)
{
    //delete currentProgram[zone];

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
            currentProgram[--zone]= z1SchedLocal; //z1Sched;
        } else if (zone  == 2){
            Serial.printf("z2 schedule selected.\n" );
            currentProgram[--zone]= z2SchedLocal; //z2Sched;
        }
    }
}

void Programmer::getCurrentSetpoint(int zone, time_t now, struct SetPoint& setpoint)
{
    Serial.printf("Getting current set point for zone %d\n", zone);
    return currentProgram[--zone]->getCurrentSetpoint(now, setpoint);
}

/*void Programmer::getCurrentSchedule(int zone, int day, float* setPoints)
{
    Serial.printf("Getting current schedule for zone %d and day %d\n", zone, day);
    return currentProgram[zone]->getSchedule(day, setPoints);
}*/

void Programmer::getSchedule(int schedule, int day, float* setPoints)
{
    if(schedule == 1) {
        z1SchedLocal->getSchedule(day, setPoints);
    } else if(schedule == 2) {
        Serial.printf("getting zone 2 schedule.\n" );
        z2SchedLocal->getSchedule(day, setPoints);
    }
}
void Programmer::updateTemp(int schedule, int day, int hour, float temperature)
{
    if(schedule == 1) {
        z1SchedLocal->updateTemp(day, hour, temperature);
    } else if(schedule == 2) {
        z2SchedLocal->updateTemp(day, hour, temperature);
    }
}
// ========================================

ScheduleProg::ScheduleProg()
{
    Serial.printf("ScheduleProg constructor.\n" );
    for(int j=0;j<7;j++)
    {
        for(int i = 0; i<24; i++)
        {
            ScheduledTemps[j][i] = 21.0f;
        }
    }
}

void ScheduleProg::getSchedule(int day, float* setPoints)
{
    --day;
    for(int i = 0; i<24; i++)
    {
        setPoints[i] = ScheduledTemps[day][i];
    }
}

void ScheduleProg::updateTemp(int day, int hour, float temperature)
{
    --day;
    Serial.printf("updateTemp Schedule: day: %d, hour: %d, temp: %3.2f\n",
         day,  hour,  temperature);
    ScheduledTemps[day][hour] = temperature;
}

void ScheduleProg::getCurrentSetpoint(time_t now, struct SetPoint& setpoint)
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
