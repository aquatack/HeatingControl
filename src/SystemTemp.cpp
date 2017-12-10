
float getTempDegC(int adcInput)
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
