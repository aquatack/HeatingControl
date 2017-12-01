
#define BLYNK_PRINT Serial  // Set serial output for debug prints
//#define BLYNK_DEBUG       // Uncomment this to see detailed prints

#include <blynk.h>
#include "BlynkAuth.h"

int i = 0, j = 0;
float remoteTemperature = -999;
time_t timeStamp = 0;

// Cloud functions must return int and take one String
int retrieveRemoteTemperature(String extra) {
    remoteTemperature = atof(extra);
    timeStamp = Time.now();
    return 1;
}

// Attach a Button widget (mode: Push) to the Virtual pin 1 - and send sweet tweets!
BLYNK_WRITE(V4) {
    j = param.asInt();
}

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
}

void loop()
{
    Serial.println("******************");

    i++;
    Blynk.virtualWrite(V2, i);
    Serial.printf("%d\n", j);

    Serial.printf("remoteTemp at %i: %3.4f \n", timeStamp, remoteTemperature);
    int age = Time.now() - timeStamp;
    Serial.printf("Age: %d\n", age);

    Blynk.run();
    delay(1000);

}
