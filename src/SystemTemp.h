#ifndef SYSTEMTEMP_H
#define SYSTEMTEMP_H

class RemoteTemp {
private:
    // Time after which we consider the remote temperature to be invalid (ms)
    const long InvalidationTime = 120 * 1000;
public:
    time_t timestamp;
    float temperature;

    bool validTemp(time_t now)
    {
        if(timestamp+InvalidationTime > now)
            return true;
        else
            return false;
    }
};

// Converts the ADC input value into a float temperature in deg C.
float getTempDegC(int adcInput);

#endif
