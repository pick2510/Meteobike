#include <thread>
#include <atomic>
#include <chrono>
#include "pi_2_dht_read.h"

#include "dhtpoller.h"
#include "consts.h"

dhtpoller::dhtpoller(const int pin) : temperature(-99), humidity(-99), pin(pin), t_humidity(-99), t_temperature(-99)
{
}

dhtpoller::~dhtpoller()
{
}

int dhtpoller::readDHT()
{

    // Set GPIO pin to output
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_gpio_write(pin, HIGH);
    usleep(500000); // 500 ms
    bcm2835_gpio_write(pin, LOW);
    usleep(20000);

    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    // wait for pin to drop?
    while (bcm2835_gpio_lev(pin) == 1)
    {
        usleep(1);
    }

    // read data!
    for (int i = 0; i < MAXTIMINGS; i++)
    {
        counter = 0;
        while (bcm2835_gpio_lev(pin) == laststate)
        {
            counter++;
            // nanosleep(1);		// overclocking might change this?
            if (counter == 1000)
                break;
        }
        laststate = bcm2835_gpio_lev(pin);
        if (counter == 1000)
            break;
        bits[bitidx++] = counter;

        if ((i > 3) && (i % 2 == 0))
        {
            // shove each bit into the storage bytes
            data[j / 8] <<= 1;
            if (counter > 200)
                data[j / 8] |= 1;
            j++;
        }
    }

    if ((j >= 39) &&
        (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)))
       {
 
            
            h = data[0] * 256 + data[1];
            h /= 10;

            f = (data[2] & 0x7F) * 256 + data[3];
            f /= 10.0;
            if (data[2] & 0x80)
                f *= -1;
            t_humidity = h;
            t_temperature = f;
        }
        return 1;

    return 0;
}

std::tuple<float, float> dhtpoller::getMeasurement()
{
    return std::make_tuple(temperature, humidity);
}

void dhtpoller::startPoll(std::atomic<bool> *signal)
{
    for (;;)
    {
        if (int retval = pi_2_dht_read(DHT22,pin, &t_humidity, &t_temperature) != 0)
        {
            std::cerr << "No data, keeping most recent data" << std::endl;
            if (signal->load())
                break;
            continue;
        }
        else
        {
            if (t_humidity > 100 | t_humidity < 0 | t_temperature<-10 | t_temperature> 60)
                continue;
            const std::scoped_lock<std::mutex> lock(l_dht);
            temperature = t_temperature;
            humidity = t_humidity;
        }
        std::this_thread::sleep_for(std::chrono::seconds(DHT_SLEEP));
        if (signal->load())
            break;
    }
}

dhtdata_r dhtpoller::getLatestData()
{
    std::scoped_lock<std::mutex> lock(l_dht);
    dhtdata_r mydata;
    mydata.humdidity = humidity;
    mydata.temperature = temperature;
    return mydata;
}