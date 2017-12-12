#include "Programmer.h"

float ScheduledTemps[24] = {0};

struct TempItem {
    TempItem* next;
    int hour;
    float temp;
}

class TempList {
    TempItem * start = NULL;
public:
    void addItem(int hour, float temp)
    {
        TempItem* item = new TempItem();
        item->hour = hour;
        item->temp = temp;
        item->next = NULL;
        if(start == NULL)
        {
            start = item;
            return;
        }
        TempItem* currentItem = start->next;
        while(currentItem != NULL && currentItem->hour<hour)
        {
            currentItem = currentItem->next;
        }
        TempItem* newNext = currentItem->next;
        currentItem->next = item;
        item->next = newNext;        
    }
}

void initialiseScheduledTemps()
{
    for(int i = 0; i<24; i++)
    {
        ScheduledTemps[i] = 21.0f;
    }
}

void getSchedule(int schedule, int zone, int day, float* setPoints)
{
    for(int i = 0; i<24; i++)
    {
        setPoints[i] = ScheduledTemps[i];
    }
    //Blynk.virtualWrite(V1, "clr");
}

void getCurrentSetpoint(time_t now, struct SetPoint& setpoint)
{
    int weekday = Time.weekday(now);
    int hour = Time.hour(now);

    setpoint.intended = ScheduledTemps[hour];
}
