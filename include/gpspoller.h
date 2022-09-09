#pragma once
#include <string>
#include <atomic>
#include <mutex>
#include "libgpsmm.h"

struct gpsdata_r {
    std::string time;
    float longitude,latitude,altitude;
    bool has_fix;
};


class gpspoller
{
private:
   std::mutex g_i_mutex; 
   std::string gps_time;
   bool has_fix;
   float altitude,latitude,longitude;
   const int port;
   gpsmm *gps;
   struct gps_data_t* data;
   enum TimeFormat { LOCALTIME, UTC, UNIX, ISO_8601 };
public:
    gpspoller(const std::string &host, const int port=2947);
    std::string TimespecToTimeStr(const timespec& gpsd_time, TimeFormat time_format = LOCALTIME);
    void startPoll(std::atomic<bool> *signal);
    gpsdata_r getLastData();
    ~gpspoller();
};


