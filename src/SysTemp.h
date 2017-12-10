#ifndef SYSTEMP_H
#define SYSTEMP_H


#include <blynk.h>

class SystemTemperature {
    static float getTempDegC(int adcInput);
public:
    static void measureSysTemp();
};

#endif
