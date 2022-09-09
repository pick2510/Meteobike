#include <cmath>
#include "measurement.h"

const double measurement::HI_C1 = -8.78469475556;
const double measurement::HI_C2 = 1.61139411;
const double measurement::HI_C3 = 2.33854883889;
const double measurement::HI_C4 = -0.14611605;
const double measurement::HI_C5 = -0.012308094;
const double measurement::HI_C6 = -0.0164248277778;
const double measurement::HI_C7 = 2.211732e-3;
const double measurement::HI_C8 = 7.2546e-4;
const double measurement::HI_C9 = -3.582e-6;

measurement::measurement(const gpsdata_r gpsdata, const dhtdata_r dhtdata) : gps(gpsdata), dht(dhtdata), pv(0), heatindex(-99), wbtemp(0)
{
    calculateVapPres();
    calculateHeatIndex();
    calculateWetBulbTemp();
}

measurement::~measurement()
{
}

void measurement::calculateVapPres()
{
    // Magnus formula
    if (dht.temperature < 0)
    {
        pv = (dht.humdidity / 100) * (610.5 * exp((22.5 * dht.temperature) / (273 + dht.temperature)));
    }
    else
    {
        pv = (dht.humdidity / 100) * (610.5 * exp((17.27 * dht.temperature) / (237.3 + dht.temperature)));
    }
}

void measurement::calculateHeatIndex()
{
    // Official Heatindex formula with constants statically defined. >= 26.7° and >= 40%
    if (dht.temperature > 26.69 && dht.humdidity > 39.9)
    {
        heatindex = HI_C1 + HI_C2 * dht.temperature + HI_C3 * dht.humdidity + HI_C4 * dht.temperature * dht.humdidity + HI_C5 * pow(dht.temperature, 2) + HI_C6 * pow(dht.humdidity, 2) + HI_C7 * pow(dht.temperature, 2) * dht.humdidity + HI_C8 * dht.temperature * pow(dht.humdidity, 2) + HI_C9 * pow(dht.temperature, 2) * pow(dht.humdidity, 2);
    }
}

void measurement::calculateWetBulbTemp()
{
    // https://doi.org/10.1175/JAMC-D-11-0143.1
    //  Wet-Bulb Temperature from Relative Humidity and Air Temperature, Roland Stull
    wbtemp = dht.temperature * atan(0.151977 * pow((dht.humdidity + 8.313659), 0.5)) + atan(dht.temperature + dht.humdidity) - atan(dht.humdidity - 1.676331) + 0.00391838 * pow(dht.humdidity, 1.5) * atan(0.023101 * dht.humdidity) - 4.686035;
}

results measurement::retres()
{
    results res = {.gps = gps, .dht = dht, .pv = pv, .heatindex = heatindex, .wbtemp = wbtemp};
    return res;
}