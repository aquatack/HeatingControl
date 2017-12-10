
#define BLYNK_PRINT Serial  // Set serial output for debug prints
//#define BLYNK_DEBUG       // Uncomment this to see detailed prints

#include <blynk.h>
#include "BlynkAuth.h"
#include "SystemTemp.h"
#include "ZoneController.h"
#include "Programmer.h"

// Blynk defines
#define SYS_TEMP V0
#define Z1_TEMP V1
#define Z2_TEMP V2
#define Z1_SETPOINT V3
#define Z2_SETPOINT V4

// Particle IO defines
#define A_SYSTEMTEMP A0
#define D_ZONE1_CTRL D0
#define D_ZONE2_CTRL D1

// Time in s that we want Z2 temp reading to be within.
const int   MAX_AGE_Z2_TEMP = 5*60;
// Timing interval to capture the system temperature (s).
const long  SYS_TEMP_CAPTURE_INTERVAL = 1*1000;

RemoteTemp      zone1Temp;
RemoteTemp      zone2Temp;
ZoneController  zone1Controller = ZoneController(D_ZONE1_CTRL);
ZoneController  zone2Controller = ZoneController(D_ZONE2_CTRL);
SetPoint        z1SetPoint;
SetPoint        z2SetPoint;
BlynkTimer      timer;

// Cloud functions must return int and take one String
int retrieveZone2Temperature(String extra) {
    zone2Temp.temperature = atof(extra);
    zone2Temp.timestamp = Time.now();
    Serial.printf("remoteTemp at %i: %3.2fC.\n", zone2Temp.timestamp, zone2Temp.temperature);
    Blynk.virtualWrite(Z2_TEMP, zone2Temp.temperature);

    ControllerState state;
    zone2Controller.UpdateSystem(zone2Temp.timestamp, zone2Temp, z2SetPoint, state);
    return 1;
}

// Call back for the setpoint.
BLYNK_WRITE(Z1_SETPOINT) {
    z1SetPoint.intended = param.asFloat();
    Serial.printf("Z1 Setpoint: %f\n", z1SetPoint.intended);
    ControllerState state;
    zone2Controller.UpdateSystem(Time.now(), zone1Temp, z1SetPoint, state);
}

// Call back for the setpoint.
BLYNK_WRITE(Z2_SETPOINT) {
    z2SetPoint.intended = param.asFloat();
    Serial.printf("Z2 Setpoint: %f\n", z2SetPoint.intended);
    ControllerState state;
    zone2Controller.UpdateSystem(Time.now(), zone2Temp, z2SetPoint, state);
}

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

    // Setup the Controllers.
    time_t t = Time.now();
    zone1Controller.InitialiseController(t);
    zone2Controller.InitialiseController(t);

    // Setup the setpoints
    z1SetPoint.intended = 21.0;
    z2SetPoint.intended = 21.0;


    // Cloud function used to retrieve a zone 2 temperature.
    zone2Temp.temperature = -999.0;
    zone2Temp.timestamp = 0;
    bool success = Particle.function("postTemp", retrieveZone2Temperature);

    delay(5000); // Allow board to settle

    // Blynk startup and publishing wakeup message.
    Particle.publish("Waking up");
    Blynk.begin(BlynkAuth);

    // sync from Blynk.
    Blynk.syncVirtual(Z1_SETPOINT);
    Blynk.syncVirtual(Z2_SETPOINT);
}

// Looper.
void loop()
{
    Blynk.run(); // updates Blynk
    timer.run(); // spins the BlynkTimer
}
