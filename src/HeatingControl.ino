
#define BLYNK_PRINT Serial  // Set serial output for debug prints
//#define BLYNK_DEBUG       // Uncomment this to see detailed prints

#include <blynk.h>
#include "BlynkAuth.h"
//#include "SystemTemp.h"

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

float GetTempDegC(int adcInput)
{
    const float offset_mV = 500.0;
    const float scale_mV_degC = 10.0;
    const float valPermV = 0.8; // 4096 steps between 0 and 3.3V

    float mVOut = adcInput * valPermV;

    // mVOut = offset_mV + T * scale_mV_degC
    // (mVOut - offset_mV)/scale_mV_degC

    float temperature = (mVOut - offset_mV) / scale_mV_degC;
    return temperature;
}

// Cloud functions must return int and take one String
int retrieveRemoteTemperature(String extra) {
    remoteTemperature = atof(extra);
    timeStamp = Time.now();
    Serial.printf("remoteTemp at %i: %3.2fC.\n", timeStamp, remoteTemperature);
    Blynk.virtualWrite(Z2_TEMP, remoteTemperature);
    return 1;
}

// Attach a Button widget (mode: Push) to the Virtual pin 1 - and send sweet tweets!
BLYNK_WRITE(Z2_SETPOINT) {
    z2SetPoint = param.asInt();
    Serial.printf("Setpoint: %f\n", z2SetPoint);
}

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void measureSysTemp()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  delay(100); // allow the ADC to settle.
  float systemTemp = GetTempDegC(analogRead(A_SYSTEMTEMP));
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
    bool success = Particle.function("postTemp", retrieveRemoteTemperature);

    delay(5000); // Allow board to settle

    timer.setInterval(SYS_TEMP_CAPTURE_INTERVAL, measureSysTemp);

    Particle.publish("Waking up");
    Blynk.begin(BlynkAuth);

    // sync the setpoint temperature.
    Blynk.syncVirtual(Z2_SETPOINT);
}

// Looper.
void loop()
{



    Blynk.run();
    timer.run(); // spins the BlynkTimer
    //delay(1000);

}
