#pragma once
#include <memory>
#include "ts_queue.h"
#include "gpspoller.h"
#include "measurement.h"
#include "EPD_2in7.h"
#include "GUI_Paint.h"

class displayupdater
{
private:
    threadsafe_queue<results> &queue;
    std::unique_ptr<UBYTE> image;
public:
    displayupdater(threadsafe_queue<results> &inqueue, std::unique_ptr<UBYTE> image);
    ~displayupdater();
};


