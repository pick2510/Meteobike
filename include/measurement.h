#pragma once

#include "gpspoller.h"
#include "dhtpoller.h"


struct results_r {
    gpsdata_r gps;
    dhtdata_r dht;
    double pv,heatindex,wbtemp;
    bool is_ending;
};

class measurement

{
private:
    static const double HI_C1,HI_C2,HI_C3,HI_C4,HI_C5,HI_C6,HI_C7,HI_C8,HI_C9;
    gpsdata_r gps;
    dhtdata_r dht;
    double pv;
    double heatindex;
    double wbtemp;
    void calculateVapPres();
    void calculateHeatIndex();
    void calculateWetBulbTemp();
public:
    measurement(const gpsdata_r gpsdata, const dhtdata_r dhtdata);
    results_r retres();
    ~measurement();
};

