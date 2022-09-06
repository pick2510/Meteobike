#include "dht_measurement.h"
#include "pi_2_dht_read.h"

dht_measurement::dht_measurement(const int pin):
temperature(-99), humidity(-99), pin(pin)
{
}

dht_measurement::~dht_measurement()
{
}

int dht_measurement::measure(){
    return pi_2_dht_read(DHT22,pin,&humidity,&temperature);
}

std::tuple<float,float> dht_measurement::getMeasurement(){
    return std::make_tuple(temperature, humidity);
}