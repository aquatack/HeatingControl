#include "Particle.h"
#include "ZoneController.h"

ZoneController::ZoneController(int zoneControlOutput)
{
    ZoneControlOutput = zoneControlOutput;
}

void ZoneController::InitialiseController(time_t now)
{
    LastHeatingToggleTime = now;
}

void ZoneController::UpdateSystem(time_t now, RemoteTemp &measuredTemperature, SetPoint setPoint, ControllerState &state)
{
    state.measuredTemp = &measuredTemperature;
    state.setPoint.intended = setPoint.intended;
    state.setPoint.intendedH = setPoint.intended + HysterisisBracketSide;
    state.setPoint.intendedL = setPoint.intended - HysterisisBracketSide;
    state.zoneOn = digitalRead(ZoneControlOutput);
    state.zoneIntent = digitalRead(ZoneControlOutput);
    int backoff = LastHeatingToggleTime + MinSwitchTime - now;
    state.zoneBackoffT = max(backoff, 0);

    if(!measuredTemperature.validTemp(now))
    {
        Serial.printf("The temperature measurement is too old (measure: %d, now: %d). Ignoring.\n",
            measuredTemperature.timestamp,
            now);
        return;
    }

    if(!digitalRead(ZoneControlOutput) && measuredTemperature.temperature < state.setPoint.intendedL)
    {
        Serial.printf("Measured: %3.2f, Target: %3.2f. Too Cold. Trying to switch on Zone %d.\n",
            measuredTemperature.temperature,
            state.setPoint.intended,
            ZoneControlOutput+1);
        state.zoneBackoffT = SwitchHeating(now, true);
        state.zoneIntent = true;
        state.zoneOn = digitalRead(ZoneControlOutput);
        return;
    }
    else if(digitalRead(ZoneControlOutput) && measuredTemperature.temperature > state.setPoint.intendedH)
    {
        Serial.printf("Measured: %3.2f, Target: %3.2f. Too Hot. Trying to switch off Zone %d.\n",
            measuredTemperature.temperature,
            state.setPoint.intended,
            ZoneControlOutput+1);
        state.zoneBackoffT = SwitchHeating(now, false);
        state.zoneIntent = false;
        state.zoneOn = digitalRead(ZoneControlOutput);
        return;
    }
    return;
}

int ZoneController::SwitchHeating(time_t now, bool on)
{
    if(LastHeatingToggleTime + MinSwitchTime < now)
    {
        if(on)
        {
            Serial.printf("SWITCHING ON ZONE %d.\n", ZoneControlOutput);
            digitalWrite(ZoneControlOutput, HIGH);
        }
        else
        {
            Serial.printf("SWITCHING OFF ZONE %d.\n", ZoneControlOutput);
            digitalWrite(ZoneControlOutput, LOW);
        }

        LastHeatingToggleTime = now;
        return -1;
    }

    int delay = LastHeatingToggleTime + MinSwitchTime - now;
    Serial.printf("Switching hold-off: %d.\n", delay);

    return delay;
}
