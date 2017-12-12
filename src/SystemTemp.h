#ifndef SYSTEMTEMP_H
#define SYSTEMTEMP_H

struct RemoteTemp {
    time_t timestamp;
    float temperature;
};

// Converts the ADC input value into a float temperature in deg C.
float getTempDegC(int adcInput);

#endif
