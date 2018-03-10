#include "Programmer.h"

const float OFF_TEMP = 5.0f;
const float ON_TEMP  = 50.0f;

// ToDo: make static.
Programmer::Programmer()
{
    offProgLocal = new OffProg();
    onProgLocal = new OnProg();
    z1SchedLocal = new ScheduleProg();
    z2SchedLocal = new ScheduleProg();
    awayProg = new AwayProg();
    tempProg[0] = new TemporaryProg();
    tempProg[1] = new TemporaryProg();
    previousProg[0] = NULL;
    previousProg[1] = NULL;
}

// zero indexed zone.
void Programmer::selectProgram(int zone, ProgramIds programId)
{
    Serial.printf("selecting a program: zone %d, progId %d\n", zone, programId);
    if(programId == ProgramIds::Off)
    {
        Serial.printf("off prog selected.\n" );
        currentProgram[zone] = offProgLocal;
    }
    else if(programId == ProgramIds::On)
    {
        Serial.printf("On prog selected.\n" );
        currentProgram[zone] = onProgLocal;
    }
    else if(programId == ProgramIds::Schedule)
    {
        if(zone == 0)
        {
            Serial.printf("z1 schedule selected.\n");
            currentProgram[zone] = z1SchedLocal;
        }
        else if (zone == 1)
        {
            Serial.printf("z2 schedule selected.\n");
            currentProgram[zone] = z2SchedLocal;
        }
    }
    else if(programId == ProgramIds::Away)
    {
        Serial.printf("Away prog selected.\n" );
        currentProgram[zone]= awayProg;
    }
    else if (programId == ProgramIds::OneHrOverride)
    {
        if(zone == 0)
        {
            Serial.printf("z1 1hr prog selected.\n");
            currentProgram[0] = tempProg[0];
        }
        else if (zone == 1)
        {
            Serial.printf("z2 1hr prog selected.\n");
            currentProgram[1] = tempProg[1];
        }
    }
}

int Programmer::getProgramId(int zone)
{
    return currentProgram[zone]->getProgramId();
}

void Programmer::setOverride(int zone, float temp)
{
    int refZone = zone;
    Serial.printf("in Programmer::setOverride. Zone: %d\n", refZone);
    if(previousProg[refZone] == NULL)
    {
        Serial.printf("Programmer::SetOverride: updating proviousProg\n");
        previousProg[refZone] = currentProgram[refZone];
        currentProgram[refZone] = tempProg[refZone];
    }
    tempProg[refZone]->SetSetpoint(temp);
}

void Programmer::resetOverride(int zone)
{
    if(previousProg[zone] != NULL)
    {
        currentProgram[zone] = previousProg[zone];
        previousProg[zone] = NULL;
    }
}

void Programmer::getCurrentSetpoint(int zone, time_t now, struct SetPoint& setpoint)
{
    Serial.printf("Getting current set point for zone %d\n", zone);
    return currentProgram[zone]->getCurrentSetpoint(now, setpoint);
}

void Programmer::getSchedule(int schedule, int day, ProgramPoints* programPoints)
{
    if(schedule == Schedules::Zone1) {
        Serial.printf("getting zone 1 schedule.\n" );
        z1SchedLocal->getSchedule(day, programPoints);
    } else if(schedule == Schedules::Zone2) {
        Serial.printf("getting zone 2 schedule.\n" );
        z2SchedLocal->getSchedule(day, programPoints);
    } else if(schedule == Schedules::AwaySched) {
        awayProg->getSchedule(day, programPoints);
    }

    Serial.printf("requested day index: %d. starttime: %d, temp: %f\n", day, programPoints->startTime[1], programPoints->targetTemp[1]);
}
void Programmer::updateTemp(int schedule, int day, int progIndex, float temperature)
{
    if(schedule == Schedules::Zone1) {
        z1SchedLocal->updateTemp(day, progIndex, temperature);
    } else if(schedule == Schedules::Zone2) {
        z2SchedLocal->updateTemp(day, progIndex, temperature);
    } else if(schedule == Schedules::AwaySched) {
        awayProg->updateTemp(day, progIndex, temperature);
    }
}
void Programmer::updateTime(int schedule, int day, int progIndex, long setTime)
{
    if(schedule == Schedules::Zone1) {
        z1SchedLocal->updateTime(day, progIndex, setTime);
    } else if(schedule == Schedules::Zone2) {
        z2SchedLocal->updateTime(day, progIndex, setTime);
    } else if(schedule == Schedules::AwaySched) {
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

int ScheduleProg::getProgramId()
{
    return ProgramIds::Schedule;
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

int AwayProg::getProgramId()
{
    return (int)ProgramIds::Away;
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
    while(temperatureProgram[weekday].startTime[schedIndex] <= nowInSecs && schedIndex < 7)
    {
        selectedTemp = temperatureProgram[weekday].targetTemp[schedIndex];
        schedIndex++;
    }

    Serial.printf("requested time: %d, %d: value: %3.2f\n",
        weekday, hour, selectedTemp);
    setpoint.intended = selectedTemp;
}

int OffProg::getProgramId()
{
    return (int)ProgramIds::Off;
}

void OffProg::getCurrentSetpoint(time_t now, struct SetPoint& setpoint)  {
    Serial.printf("In OffProg::getCurrentSetpoint.\n");
    setpoint.intended = OFF_TEMP;
}

int OnProg::getProgramId()
{
    return (int)ProgramIds::On;
}

void OnProg::getCurrentSetpoint(time_t now, struct SetPoint& setpoint)  {
    Serial.printf("In OnProg::getCurrentSetpoint.\n");
    setpoint.intended = ON_TEMP;
}

int TemporaryProg::getProgramId()
{
    return (int)ProgramIds::OneHrOverride;
}

void TemporaryProg::SetSetpoint(float temperature)
{
    Serial.printf("In TemporaryProg::SetSetpoint. Setting %3.2f\n",temperature);
    setPoint = temperature;
}
void TemporaryProg::getCurrentSetpoint(time_t now, struct SetPoint& setpoint)  {
    Serial.printf("In OnProg::getCurrentSetpoint.\n");
    setpoint.intended = setPoint;
}
