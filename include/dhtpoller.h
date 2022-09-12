#pragma once

#include <tuple>
#include <atomic>
#include <mutex>
#include "bcm2835.h"

struct dhtdata_r{
    float temperature, humdidity;
};


class dhtpoller
{
private:
    std::mutex l_dht;
    float temperature, humidity, t_temperature,t_humidity;
    int pin;
    int counter = 0;
    int laststate = HIGH;
    int j = 0;
    float f=0, h=0;
    int bits[249], data[100];
    int bitidx = 0;
public:
    dhtpoller(const int pin);
    ~dhtpoller();
    int readDHT();
    void startPoll(std::atomic<bool> *signal);
    std::tuple<float,float> getMeasurement();
    dhtdata_r getLatestData();
};

