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

void Programmer::getSchedule(int schedule, int day, ProgramPoints* programPoints)
{
    if(schedule == 1) {
        z1SchedLocal->getSchedule(day, programPoints);
    } else if(schedule == 2) {
        Serial.printf("getting zone 2 schedule.\n" );
        z2SchedLocal->getSchedule(day, programPoints);
    } else if(schedule == 3) {
        awayProg->getSchedule(day, programPoints);
    }

    Serial.printf("requested day index: %d. starttime: %d, temp: %f\n", day, programPoints->startTime[1], programPoints->targetTemp[1]);
}
void Programmer::updateTemp(int schedule, int day, int progIndex, float temperature)
{
    if(schedule == 1) {
        z1SchedLocal->updateTemp(day, progIndex, temperature);
    } else if(schedule == 2) {
        z2SchedLocal->updateTemp(day, progIndex, temperature);
    } else if(schedule == 3) {
        awayProg->updateTemp(day, progIndex, temperature);
    }
}
void Programmer::updateTime(int schedule, int day, int progIndex, long setTime)
{
    if(schedule == 1) {
        z1SchedLocal->updateTime(day, progIndex, setTime);
    } else if(schedule == 2) {
        z2SchedLocal->updateTime(day, progIndex, setTime);
    } else if(schedule == 3) {
        awayProg->updateTime(day, progIndex, setTime);
    }
}
// ========================================

ScheduleProg::ScheduleProg()
{
    Serial.printf("ScheduleProg constructor.\n" );
    for(int j=0;j<7;j++)
    {
        temperatureProgram[j].startTime[0] = 0;
        temperatureProgram[j].targetTemp[0] = 20.0;
        temperatureProgram[j].startTime[1] = 3 * 60 * 60;
        temperatureProgram[j].targetTemp[1] = 21.0;
        temperatureProgram[j].startTime[2] = 6 * 60 * 60;
        temperatureProgram[j].targetTemp[2] = 22.0;
        temperatureProgram[j].startTime[3] = 8.5 * 60 * 60;
        temperatureProgram[j].targetTemp[3] = 17.0;
        temperatureProgram[j].startTime[4] = 17 * 60 * 60;
        temperatureProgram[j].targetTemp[4] = 21.0;
        temperatureProgram[j].startTime[5] = 18.5 * 60 * 60;
        temperatureProgram[j].targetTemp[5] = 22.5;
        temperatureProgram[j].startTime[6] = 23.75 * 60 * 60;
        temperatureProgram[j].targetTemp[6] = 20.0;
    }
}

AwayProg::AwayProg()
{
    float awayTemp = 15.0;
    for(int j=0;j<7;j++)
    {
        temperatureProgram[j].startTime[0] = 0;
        temperatureProgram[j].targetTemp[0] = awayTemp;
        temperatureProgram[j].startTime[1] = 5 * 60 * 60;
        temperatureProgram[j].targetTemp[1] = awayTemp;
        temperatureProgram[j].startTime[2] = 8.5 * 60 * 60;
        temperatureProgram[j].targetTemp[2] = awayTemp;
        temperatureProgram[j].startTime[3] = 17 * 60 * 60;
        temperatureProgram[j].targetTemp[3] = awayTemp;
        temperatureProgram[j].startTime[4] = 18 * 60 * 60;
        temperatureProgram[j].targetTemp[4] = awayTemp;
        temperatureProgram[j].startTime[5] = 23.5 * 60 * 60;
        temperatureProgram[j].targetTemp[5] = awayTemp;
        temperatureProgram[j].startTime[6] = 23.5 * 60 * 60;
        temperatureProgram[j].targetTemp[6] = awayTemp;
    }
}

void ProgrammableProg::getSchedule(int day, ProgramPoints* programPoints)
{
    --day;
    for(int i = 0; i < 7; i++)
    {
        programPoints->startTime[i] = temperatureProgram[day].startTime[i];
        programPoints->targetTemp[i] = temperatureProgram[day].targetTemp[i];
    }
}

void ProgrammableProg::updateTemp(int day, int progIndex, float temperature)
{
    --day;
    Serial.printf("updateTemp Schedule: day: %d, index: %d, temp: %3.2f\n",
         day,  progIndex,  temperature);
    temperatureProgram[day].targetTemp[progIndex] = temperature;
}
void ProgrammableProg::updateTime(int day, int progIndex, long setTime)
{
    --day;
    temperatureProgram[day].startTime[progIndex] = setTime;
}

void ProgrammableProg::getCurrentSetpoint(time_t now, struct SetPoint& setpoint)
{
    Serial.printf("In ScheduleProg::getCurrentSetpoint.\n");
    int weekday = Time.weekday(now) - 2;
    if(weekday < 0) { // adjust for Sunday
        weekday = 6;
    }
    int hour = Time.hour(now);
    int minute = Time.minute(now);
    long nowInSecs = minute * 60 + hour * 3600;
    int schedIndex = 0;
    float selectedTemp = 0;
    while(temperatureProgram[weekday].startTime[schedIndex] < nowInSecs && schedIndex < 7)
    {
        selectedTemp = temperatureProgram[weekday].targetTemp[schedIndex];
        schedIndex++;
    }

    Serial.printf("requested time: %d, %d: value: %3.2f\n",
        weekday, hour, selectedTemp);
    setpoint.intended = selectedTemp;
}

void OffProg::getCurrentSetpoint(time_t now, struct SetPoint& setpoint)  {
    Serial.printf("In OffProg::getCurrentSetpoint.\n");
    setpoint.intended = 0.0f;
}
void OffProg::getSchedule(int day, ProgramPoints* programPoints)  {
    float offTemp = 0.00f;
    programPoints->startTime[0] = 0;
    programPoints->targetTemp[0] = offTemp;
    programPoints->startTime[1] = 0 * 60 * 60;
    programPoints->targetTemp[1] = offTemp;
    programPoints->startTime[2] = 0 * 60 * 60;
    programPoints->targetTemp[2] = offTemp;
    programPoints->startTime[3] = 0 * 60 * 60;
    programPoints->targetTemp[3] = offTemp;
    programPoints->startTime[4] = 0 * 60 * 60;
    programPoints->targetTemp[4] = offTemp;
    programPoints->startTime[5] = 0 * 60 * 60;
    programPoints->targetTemp[5] = offTemp;
    programPoints->startTime[6] = 0 * 60 * 60;
    programPoints->targetTemp[6] = offTemp;
}


void OnProg::getCurrentSetpoint(time_t now, struct SetPoint& setpoint)  {
    Serial.printf("In OnProg::getCurrentSetpoint.\n");
    setpoint.intended = 999.0f;
}
void OnProg::getSchedule(int day, ProgramPoints* programPoints)  {
    float onTemp = 100.00f;
    programPoints->startTime[0] = 0;
    programPoints->targetTemp[0] = onTemp;
    programPoints->startTime[1] = 0 * 60 * 60;
    programPoints->targetTemp[1] = onTemp;
    programPoints->startTime[2] = 0 * 60 * 60;
    programPoints->targetTemp[2] = onTemp;
    programPoints->startTime[3] = 0 * 60 * 60;
    programPoints->targetTemp[3] = onTemp;
    programPoints->startTime[4] = 0 * 60 * 60;
    programPoints->targetTemp[4] = onTemp;
    programPoints->startTime[5] = 0 * 60 * 60;
    programPoints->targetTemp[5] = onTemp;
    programPoints->startTime[6] = 0 * 60 * 60;
    programPoints->targetTemp[6] = onTemp;
}
