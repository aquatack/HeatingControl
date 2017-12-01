
#define BLYNK_PRINT Serial  // Set serial output for debug prints
//#define BLYNK_DEBUG       // Uncomment this to see detailed prints

#include <blynk.h>
#include "BlynkAuth.h"


// Attach a Button widget (mode: Switch) to the Digital pin 7 - and control the built-in blue led.
// Attach a Graph widget to Analog pin 1
// Attach a Gauge widget to Analog pin 2
int i = 0, j = 0;

void setup()
{
    Serial.begin(9600);
    delay(5000); // Allow board to settle

    Blynk.begin(BlynkAuth);
}

// Attach a Button widget (mode: Push) to the Virtual pin 1 - and send sweet tweets!
BLYNK_WRITE(V4) {
    j = param.asInt();
}

void loop()
{
    i++;
    Blynk.virtualWrite(V2, i);
    Serial.printf("%d\n", j);
    Blynk.run();
    delay(1000);
}
