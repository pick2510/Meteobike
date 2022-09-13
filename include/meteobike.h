#pragma once
#include <string>
#include <atomic>
#include <memory>
#include <fstream>
#include "consts.h"
#include "EPD_2in7.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"



void startUp(const std::string &hostname, const std::string &ip);
void setupGPIO();
KEYS checkEvent();
void parseEvent(const KEYS &key, std::atomic<bool> &writeRecord);

