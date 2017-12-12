#ifndef ZONECTRL_H
#define ZONECTRL_H

#include "SystemTemp.h"
#include "Programmer.h"

struct ControllerState
{
    RemoteTemp  measureTemp;
    SetPoint    setPoint;
    bool        zoneOn;
    bool        zoneIntent;
    int         zoneBackoffT;
};

class ZoneController
{
    const int MinSwitchTime = 300; // Minimum time allowed between switching / after powerup.
    const float HysterisisBracketSide = 0.25;
    const int   MaxTempAge = 5*60;    // Time in s that we want Z2 temp reading to be within.
    int ZoneControlOutput;
    //Timer ControllerTimer;
    time_t LastHeatingToggleTime;

    int SwitchHeating(time_t now, bool on);
public:
    ZoneController(int zoneControlOutput);
    void InitialiseController(time_t now);
    void UpdateSystem(time_t now, RemoteTemp measuredTemperature, SetPoint setPoint, ControllerState &state);
};

#endif
