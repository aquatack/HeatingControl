
#define BLYNK_PRINT Serial  // Set serial output for debug prints
//#define BLYNK_DEBUG       // Uncomment this to see detailed prints

#include <blynk.h>
#include "BlynkAuth.h"

#define Z2_SETPOINT V4
#define Z2_TEMP V2

// Time in s that we want Z2 temp reading to be within.
const int MAX_AGE_Z2_TEMP = 5*60;

float remoteTemperature = -999;
time_t timeStamp = 0;
float z2SetPoint = 0;

// Cloud functions must return int and take one String
int retrieveRemoteTemperature(String extra) {
    remoteTemperature = atof(extra);
    timeStamp = Time.now();
    return 1;
}

// Attach a Button widget (mode: Push) to the Virtual pin 1 - and send sweet tweets!
BLYNK_WRITE(Z2_SETPOINT) {
    z2SetPoint = param.asInt();
}

// bootup routines.
void setup()
{
    // Setup IO
    pinMode(A0, INPUT);     // ADC for temp sensor
    pinMode(D0, OUTPUT);    // D0 for Zone 1 control
    digitalWrite(D0, LOW);  // Zone 1 off.

    Serial.begin(9600);
    bool success = Particle.function("postTemp", retrieveRemoteTemperature);

    delay(5000); // Allow board to settle

    Particle.publish("Waking up");
    Blynk.begin(BlynkAuth);

    // sync the setpoint temperature.
    Blynk.syncVirtual(Z2_SETPOINT);
}

// Looper.
void loop()
{
    Serial.println("******************");
    Serial.printf("%f\n", z2SetPoint);
    int age = Time.now() - timeStamp;
    if(age < MAX_AGE_Z2_TEMP)
    {
        Serial.printf("remoteTemp at %i: %3.4fC. %d old.\n", timeStamp, remoteTemperature, age);
        Blynk.virtualWrite(Z2_TEMP, remoteTemperature);
    }

    Blynk.run();
    delay(1000);

}
