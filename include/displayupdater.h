#pragma once
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
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
    threadsafe_queue<results> &queue;
    UBYTE* image;
    std::string hostname, ip;
    std::ifstream therm;
    const writer &mywriter;
public:
    displayupdater(threadsafe_queue<results> &inqueue,  const std::string &hostname, const std::string &ip, const writer &mywriter);
    ~displayupdater();
};


