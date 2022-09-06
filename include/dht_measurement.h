#pragma once

#include <tuple>

class dht_measurement
{
private:
    float temperature, humidity;
    int pin;
public:
    dht_measurement(const int pin);
    ~dht_measurement();
    std::tuple<float,float> getMeasurement();
    int measure();
};

