#pragma once
#include <string>
#include <memory>
#include "consts.h"
#include "EPD_2in7.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"


std::unique_ptr<UBYTE> startUp(const std::string &hostname, const std::string &ip);
void setupGPIO();
KEYS checkEvent();
void parseEvent(const KEYS &key, bool &writeRecord);

