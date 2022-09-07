#include <thread>
#include <atomic>
#include <chrono>

#include "dhtpoller.h"
#include "pi_2_dht_read.h"
#include "consts.h"

dhtpoller::dhtpoller(const int pin):
temperature(-99), humidity(-99), pin(pin), t_humidity(-99), t_temperature(-99)
{
}

dhtpoller::~dhtpoller()
{
}

int dhtpoller::measure(){
    return pi_2_dht_read(DHT22,pin,&humidity,&temperature);
}

std::tuple<float,float> dhtpoller::getMeasurement(){
    return std::make_tuple(temperature, humidity);
}

void dhtpoller::startPoll(std::atomic<bool> *signal){
    for (;;){
        if (int retval = pi_2_dht_read(DHT22,pin, &t_humidity, &t_temperature)!=0){
            std::cerr << "No data" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(DHT_SLEEP));
            if (signal->load()) break;
            continue;        
        } else {
            const std::lock_guard<std::mutex> lock(l_dht);
            temperature = t_temperature;
            humidity = t_humidity;
        }
        std::this_thread::sleep_for(std::chrono::seconds(DHT_SLEEP));
        if (signal->load()) break;
    }
}

dhtdata_r dhtpoller::getLatestData(){
    std::lock_guard<std::mutex> lock(l_dht);
    dhtdata_r mydata;
    mydata.humdidity = humidity;
    mydata.temperature = temperature;
    return mydata;
}