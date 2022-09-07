#pragma once

#include <tuple>
#include <atomic>
#include <mutex>

struct dhtdata_r{
    float temperature, humdidity;
};


class dhtpoller
{
private:
    std::mutex l_dht;
    float temperature, humidity, t_temperature,t_humidity;
    int pin;
public:
    dhtpoller(const int pin);
    ~dhtpoller();
    void startPoll(std::atomic<bool> *signal);
    std::tuple<float,float> getMeasurement();
    dhtdata_r getLatestData();
    int measure();
};

