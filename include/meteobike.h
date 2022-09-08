#pragma once
#include <string>
#include <memory>
#include "EPD_2in7_V2.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"


std::unique_ptr<UBYTE> startUp(const std::string &hostname, const std::string &ip);

