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
    const int MinSwitchTime = 10; // Minimum time allowed between switching / after powerup.
    const float HysterisisBracketSide = 0.25;
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
