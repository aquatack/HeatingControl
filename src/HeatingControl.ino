
#define BLYNK_PRINT Serial  // Set serial output for debug prints
//#define BLYNK_DEBUG       // Uncomment this to see detailed prints

#include <blynk.h>
#include "BlynkAuth.h"
#include "SystemTemp.h"

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

float       remoteTemperature = -999;
time_t      timeStamp = 0;
float       z2SetPoint = 0;
BlynkTimer  timer;

// Cloud functions must return int and take one String
int retrieveRemoteTemperature(String extra) {
    remoteTemperature = atof(extra);
    timeStamp = Time.now();
    Serial.printf("remoteTemp at %i: %3.2fC.\n", timeStamp, remoteTemperature);
    Blynk.virtualWrite(Z2_TEMP, remoteTemperature);
    return 1;
}

// Call back for the setpoint.
BLYNK_WRITE(Z2_SETPOINT) {
    z2SetPoint = param.asInt();
    Serial.printf("Setpoint: %f\n", z2SetPoint);
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

    // Cloud function used to retrieve a remote temperature.
    bool success = Particle.function("postTemp", retrieveRemoteTemperature);

    delay(5000); // Allow board to settle

    // System timers for events.
    timer.setInterval(SYS_TEMP_CAPTURE_INTERVAL, measureSysTemp);

    // Blynk startup and publishing wakeup message.
    Particle.publish("Waking up");
    Blynk.begin(BlynkAuth);

    // sync from Blynk.
    Blynk.syncVirtual(Z2_SETPOINT);
}

// Looper.
void loop()
{
    Blynk.run(); // updates Blynk
    timer.run(); // spins the BlynkTimer
}
