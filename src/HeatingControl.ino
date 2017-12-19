
#define BLYNK_PRINT Serial  // Set serial output for debug prints
//#define BLYNK_DEBUG       // Uncomment this to see detailed prints

#include <blynk.h>
#include "BlynkAuth.h"
#include "SystemTemp.h"
#include "ZoneController.h"
#include "Programmer.h"

// Blynk defines
#define SYS_TEMP        V0
#define Z1_TEMP         V1
#define Z2_TEMP         V2
#define Z1_SETPOINT     V3
#define Z2_SETPOINT     V4
#define Z1_SETPOINTL    V5
#define Z2_SETPOINTL    V6
#define Z1_SETPOINTH    V7
#define Z2_SETPOINTH    V8
#define Z1_ON           V9
#define Z2_ON           V10
#define Z1_INTENT       V11
#define Z2_INTENT       V12
#define Z1_BACKOFFT     V13
#define Z2_BACKOFFT     V14
#define Z1_MODE         V15
#define Z2_MODE         V16

// Blynk Programming channels.
#define PROG_ZONE_SELECT        V50
#define PROG_DAY_SELECT         V51
#define PROG_SCHEDULE_SELECT    V52
#define PROG_SCHEDULE_TABLE     V53
#define PROG_TIME_IP            V54
#define PROG_TEMP_SELECT        V55
#define PROG_ADD                V56
#define PROG_DELETE_SELECTION   V57

// Particle IO defines
#define A_SYSTEMTEMP    A0
#define D_ZONE1_CTRL    D0
#define D_ZONE2_CTRL    D1


// Timing interval to capture the system temperature (s).
const long  SYS_TEMP_CAPTURE_INTERVAL = 5 * 1000;
const long  SYS_STATE_UPDATE_INTERVAL = 1 * 1000;
const long  SETPOINT_UPDATE_INTERVAL  = 60 * 1000;

RemoteTemp      zone1Temp;
RemoteTemp      zone2Temp;
ZoneController  zone1Controller = ZoneController(D_ZONE1_CTRL);
ZoneController  zone2Controller = ZoneController(D_ZONE2_CTRL);
SetPoint        z1SetPoint;
SetPoint        z2SetPoint;
BlynkTimer      timer;

WidgetLED       z1Active(Z1_ON);
WidgetLED       z2Active(Z2_ON);
WidgetLED       z1Intent(Z1_INTENT);
WidgetLED       z2Intent(Z2_INTENT);

// Update Blynk client on temp measurement or setPoint change
void updateZ1BlynkClient(struct RemoteTemp &zoneTemp, struct ControllerState &state)
{
    Blynk.virtualWrite(Z1_TEMP, zoneTemp.temperature);
    if(state.zoneOn)
        z1Active.on();
    else
        z1Active.off();
    if(state.zoneIntent)
        z1Intent.on();
    else
        z1Intent.off();
    //Blynk.virtualWrite(Z1_BACKOFFT, state.zoneBackoffT);
    Blynk.virtualWrite(Z1_SETPOINT, state.setPoint.intended);
    Blynk.virtualWrite(Z1_SETPOINTL, state.setPoint.intendedL);
    Blynk.virtualWrite(Z1_SETPOINTH, state.setPoint.intendedH);
    return;
}

void updateZ2BlynkClient(struct RemoteTemp &zoneTemp, struct ControllerState &state)
{
    Blynk.virtualWrite(Z2_TEMP, zoneTemp.temperature);
    if(state.zoneOn)
        z2Active.on();
    else
        z2Active.off();
    if(state.zoneIntent)
        z2Intent.on();
    else
        z2Intent.off();
    //Blynk.virtualWrite(Z2_BACKOFFT, state.zoneBackoffT);
    Blynk.virtualWrite(Z2_SETPOINT, state.setPoint.intended);
    Blynk.virtualWrite(Z2_SETPOINTL, state.setPoint.intendedL);
    Blynk.virtualWrite(Z2_SETPOINTH, state.setPoint.intendedH);
    return;
}

// Cloud functions must return int and take one String
int retrieveZone1Temperature(String extra) {
    zone1Temp.temperature = atof(extra);
    zone1Temp.timestamp = Time.now();
    Serial.printf("remoteTemp at %i: %3.2fC.\n", zone1Temp.timestamp, zone1Temp.temperature);

    ControllerState state;
    zone1Controller.UpdateSystem(zone1Temp.timestamp, zone1Temp, z1SetPoint, state);
    updateZ1BlynkClient(zone1Temp, state);
    return 1;
}

int retrieveZone2Temperature(String extra) {
    zone2Temp.temperature = atof(extra);
    zone2Temp.timestamp = Time.now();
    Serial.printf("remoteTemp at %i: %3.2fC.\n", zone2Temp.timestamp, zone2Temp.temperature);

    ControllerState state;
    zone2Controller.UpdateSystem(zone2Temp.timestamp, zone2Temp, z2SetPoint, state);
    updateZ2BlynkClient(zone2Temp, state);
    return 1;
}

void updateControllers()
{
    Serial.printf("Updating Controllers.\n");
    Serial.printf("Z1 setpoint: %3.2f, Z2 setpoint: %3.2f.\n", z1SetPoint.intended, z2SetPoint.intended);
    time_t now = Time.now();
    ControllerState state;
    zone2Controller.UpdateSystem(now, zone2Temp, z2SetPoint, state);
    updateZ2BlynkClient(zone2Temp, state);
    zone1Controller.UpdateSystem(now, zone1Temp, z1SetPoint, state);
    updateZ1BlynkClient(zone1Temp, state);
}

int selectedProgrammingSchedule = 0;
int selectedProgrammingZone = 0;
int selectedProgrammingDay = 0;
bool selectedProgrammingRows[24] = {0};
int z1Mode = 0;
int z2Mode = 0;

void refreshProgrammeTable()
{
    float scheduleTemps[24] = {0};
    getSchedule(selectedProgrammingSchedule, selectedProgrammingZone, selectedProgrammingDay, scheduleTemps);
    Blynk.virtualWrite(PROG_SCHEDULE_TABLE,"clr");
    Serial.printf("day: %d\n", selectedProgrammingDay);
    for(int i = 0; i<24; i++)
    {
        String hour = String::format("%02d",i);
        String temp = String::format("%3.1f C", scheduleTemps[i]);
        Blynk.virtualWrite(PROG_SCHEDULE_TABLE, "add", i, hour, temp);
        Blynk.virtualWrite(PROG_SCHEDULE_TABLE, "deselect", i);
        selectedProgrammingRows[i] = false;
    }

    // ToDo: only pick the hour row for the current day.
    Blynk.virtualWrite(PROG_SCHEDULE_TABLE, "pick", Time.hour());
}

BLYNK_WRITE(PROG_DAY_SELECT)
{
    selectedProgrammingDay = param.asInt();
    refreshProgrammeTable();
}

BLYNK_WRITE(PROG_ZONE_SELECT)
{
    selectedProgrammingZone = param.asInt();
    refreshProgrammeTable();
}

BLYNK_WRITE(PROG_SCHEDULE_SELECT)
{
    selectedProgrammingSchedule = param.asInt();
    refreshProgrammeTable();
}

BLYNK_WRITE(Z1_MODE)
{
    z1Mode = param.asInt();
}

BLYNK_WRITE(Z2_MODE)
{
    z2Mode = param.asInt();
}

BLYNK_WRITE(PROG_SCHEDULE_TABLE) {
   String cmd = param[0].asStr();
   if (cmd == "select") {
       //row in table was selected.
       selectedProgrammingRows[param[1].asInt()] = true;
   }
   if (cmd == "deselect") {
       //row in table was deselected.
       selectedProgrammingRows[param[1].asInt()] = false;
   }
   if (cmd == "order") {
       //rows in table where reodered
       //int oldRowIndex = param[1].asInt();
       //int newRowIndex = param[2].asInt();
   }
}
BLYNK_WRITE(PROG_TEMP_SELECT)
{
    for(int i = 0; i<24; i++)
    {
        if(selectedProgrammingRows[i])
        {
            updateTemp(selectedProgrammingSchedule,selectedProgrammingZone, selectedProgrammingDay, i, param.asFloat());
            String hour = String::format("%02d",i);
            String temp = String::format("%3.1f C", param.asFloat());
            Blynk.virtualWrite(PROG_SCHEDULE_TABLE, "update", i, hour, temp);
        }
    }
    updateSetPoints();
}

/*// Call back for the setpoint.
BLYNK_WRITE(Z1_SETPOINT) {
    z1SetPoint.intended = param.asFloat();
    Serial.printf("Z1 Setpoint: %f\n", z1SetPoint.intended);
    ControllerState state;
    zone1Controller.UpdateSystem(Time.now(), zone1Temp, z1SetPoint, state);
}

// Call back for the setpoint.
BLYNK_WRITE(Z2_SETPOINT) {
    z2SetPoint.intended = param.asFloat();
    Serial.printf("Z2 Setpoint: %f\n", z2SetPoint.intended);
    ControllerState state;
    zone2Controller.UpdateSystem(Time.now(), zone2Temp, z2SetPoint, state);
    updateZ2BlynkClient(zone2Temp, state);
}*/

// Measures the System's temperature. It then updates the relevant
// Blynk virtual pin and prints some debug. This is the callback method
// from a blynk timer.
void measureSysTemp()
{
  delay(100); // allow the ADC to settle.
  float systemTemp = getTempDegC(analogRead(A_SYSTEMTEMP));
  Blynk.virtualWrite(SYS_TEMP, systemTemp);
  Serial.printf("System temp: %3.2fC\n", systemTemp);
}

void updateSetPoints()
{
    Serial.printf("UpdateSetPoints.\n");
    // Schedule = 1. Zones 1 & 2.
    getCurrentSetpoint(Time.now(), 1, 1, z1SetPoint);
    getCurrentSetpoint(Time.now(), 1, 2, z2SetPoint);
    Serial.printf("Retrieved setpoints 1: %3.2f, 2: %3.2f\n", z1SetPoint.intended, z2SetPoint.intended);
    updateControllers();
}

// bootup routines.
void setup()
{
    // Setup IO
    pinMode(A_SYSTEMTEMP, INPUT);     // ADC for temp sensor
    pinMode(D_ZONE1_CTRL, OUTPUT);    // D0 for Zone 1 control
    digitalWrite(D_ZONE1_CTRL, LOW);  // Zone 1 off.
    pinMode(D_ZONE2_CTRL, OUTPUT);    // D1 for Zone 2 control
    digitalWrite(D_ZONE2_CTRL, LOW);  // Zone 2 off.

    Serial.begin(9600);

    // System timers for events. Ignore the timerId.
    timer.setInterval(SYS_TEMP_CAPTURE_INTERVAL, measureSysTemp);
    timer.setInterval(SETPOINT_UPDATE_INTERVAL, updateSetPoints);
    //timer.setInterval(SYS_STATE_UPDATE_INTERVAL, updateControllers); //removing for now as perhaps not necessary.

    // Setup the Controllers.
    time_t t = Time.now();
    zone1Controller.InitialiseController(t);
    zone2Controller.InitialiseController(t);

    //z1SetPoint.intended = 21.0;
    //z2SetPoint.intended = 21.0;

    zone1Temp.temperature = -999.0;
    zone1Temp.timestamp = 0;

    // Cloud function used to retrieve a zone 2 temperature.
    zone2Temp.temperature = -999.0;
    zone2Temp.timestamp = 0;

    // Setup the setpoints
    initialiseScheduledTemps();
    updateSetPoints();

    bool success = Particle.function("postTemp", retrieveZone2Temperature);

    delay(5000); // Allow board to settle

    // Blynk startup and publishing wakeup message.
    Particle.publish("Waking up");
    Blynk.begin(BlynkAuth);

    // sync from Blynk.
    Blynk.syncVirtual(Z1_SETPOINT);
    Blynk.syncVirtual(Z2_SETPOINT);
    Blynk.syncVirtual(PROG_DAY_SELECT);
    Blynk.syncVirtual(PROG_SCHEDULE_SELECT);
    Blynk.syncVirtual(PROG_ZONE_SELECT);
    Blynk.syncVirtual(Z1_MODE);
    Blynk.syncVirtual(Z2_MODE);
}

// Looper.
void loop()
{
    Blynk.run(); // updates Blynk
    timer.run(); // spins the BlynkTimer
}
