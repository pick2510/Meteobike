#pragma once
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include "consts.h"
#include "ts_queue.h"
#include "gpspoller.h"
#include "measurement.h"
#include "writer.h"
#include "EPD_2in7.h"
#include "GUI_Paint.h"



class displayupdater
{
private:
    threadsafe_queue<results_r> &queue;
    std::unique_ptr<UBYTE[]> image;
    std::string hostname, ip;
    std::ifstream therm;
    int ip_counter = 0;
    const writer &mywriter;
    const std::atomic<bool> &is_writing;
    std::chrono::system_clock::time_point timespecTotime(timespec ts);
    void updateIp();
public:
    displayupdater(threadsafe_queue<results_r> &inqueue,  const std::string &hostname, const std::string &ip, const writer &mywriter, const std::atomic<bool> &is_writing);
    void startUpdating();
    ~displayupdater();
};


