#include "gpspoller.h"
#include <string>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <cstdlib>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <thread>
#include "sys/time.h"

#include "libgpsmm.h"
#include "consts.h"

using namespace std;

gpspoller::gpspoller(const string &host, const int port) : port(port), latitude(0), longitude(0), has_fix(false), has_set_systemtime(false), altitude(0), gps_time("1970-01-01T00:00:00.000Z")
{
    stringstream port_s;
    t_time.tv_nsec = 0;
    t_time.tv_sec = 0;
    port_s << port;
    gps = std::make_unique<gpsmm>(host.c_str(), port_s.str().c_str());
    if (gps->stream(WATCH_ENABLE | WATCH_JSON) == nullptr)
    {
        cout << "Error, no GPSD running" << endl;
        exit(EXIT_FAILURE);
    }
}

string gpspoller::TimespecToTimeStr(const timespec &gpsd_time, TimeFormat time_format)
{
    std::ostringstream oss;
    switch (time_format)
    {
    case LOCALTIME:
    {
        // convert timespec_t into localtime (dd-mm-YY HH:MM:SS)
        const auto tm = *std::localtime(&gpsd_time.tv_sec);
        oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
        break;
    }
    case UTC:
    {
        // convert timespec_t into UTC (dd-mm-YY HH:MM:SS)
        const auto tm = *std::gmtime(&gpsd_time.tv_sec);
        oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
        break;
    }
    case UNIX:
        // returns seconds since the Epoch
        oss << gpsd_time.tv_sec;
        break;
    case ISO_8601:
    {
        // convert timespec_t into ISO8601 UTC time (yyyy-MM-dd'T'HH:mm:ss'Z')
        constexpr size_t kScrSize{128};
        std::array<char, kScrSize> scr{};
        timespec_to_iso8601(gpsd_time, scr.data(), kScrSize);
        oss << scr.data();
        break;
    }
    }
    return oss.str();
}

void gpspoller::startPoll(atomic<bool> *signal)
{
    for (;;)
    {

        if (!gps->waiting(kWaitingTime))
        {
            continue;
        }
        if ((data = gps->read()) == nullptr)
        {
            std::this_thread::sleep_for(std::chrono::seconds(GPS_SLEEP));
            if (signal->load())
                break;
            continue;
        }
        else
        {
            const std::scoped_lock<std::mutex> lock(g_i_mutex);
            data->fix.status == STATUS_NO_FIX ? has_fix = false : has_fix = true;
            altitude = data->fix.altitude;
            longitude = data->fix.longitude;
            latitude = data->fix.latitude;
            t_time = data->fix.time;
            speed = data->fix.speed;
            gps_time = TimespecToTimeStr(data->fix.time, ISO_8601);
        }
        if (signal->load())
        {
            break;
        }
    }
}

gpsdata_r gpspoller::getLastData()
{
    if (has_fix && !has_set_systemtime){
        tzset();
        timeval tv;
        TIMESPEC_TO_TIMEVAL(&tv,&t_time);
        time_t utc = static_cast<time_t> (tv.tv_sec);
        time_t local = utc - timezone + ( daylight?3600:0 );
        tv.tv_sec = local;
        if (settimeofday(&tv, nullptr) != 0){
            cerr << "Error, cannot set time" << endl;
        }
        tm* timeinfo = localtime (&local);
        cout <<  "The current date/time is: " << asctime(timeinfo);
        has_set_systemtime = true;
    }
    const std::scoped_lock<std::mutex> lock(g_i_mutex);
    gpsdata_r retdat {.time = gps_time, .longitude=longitude, .latitude=latitude, .altitude = altitude, .speed=speed, .has_fix = has_fix};
    return retdat;
}

gpspoller::~gpspoller()
{
}