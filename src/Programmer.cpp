#include "Programmer.h"

float ScheduledTemps[7][24] = {0.0f};

void initialiseScheduledTemps()
{
    for(int j=0;j<7;j++)
    {
        for(int i = 0; i<24; i++)
        {
            ScheduledTemps[j][i] = 21.0f;
        }
    }
}

void getSchedule(int schedule, int zone, int day, float* setPoints)
{
    --day;
    for(int i = 0; i<24; i++)
    {
        setPoints[i] = ScheduledTemps[day][i];
    }
    //Blynk.virtualWrite(V1, "clr");
}

void updateTemp(int schedule, int zone, int day, int hour, float temperature)
{
    --day;
    Serial.printf("updateTemp: sched: %d, zone: %d, day: %d, hour: %d, temp: %3.2f\n", schedule,  zone,  day,  hour,  temperature);
    ScheduledTemps[day][hour] = temperature;
}

void getCurrentSetpoint(time_t now, int schedule, int zone, struct SetPoint& setpoint)
{
    int weekday = Time.weekday(now) - 2;
    if(weekday < 0) { // adjust for Sunday
        weekday = 6;
    }
    int hour = Time.hour(now);
    Serial.printf("requested time: %d, %d\n", weekday, hour);
    setpoint.intended = ScheduledTemps[weekday][hour];
}
