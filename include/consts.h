#ifndef _CONSTS
#define _CONSTS


#include <string>
#include <iostream>
#include "EPD_2in7_V2.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"


constexpr int REFRESH = 5;
constexpr int PIN = 4;
constexpr int DHT_SLEEP = 2;
constexpr int GPS_SLEEP = 1;
constexpr UWORD Imagesize = ((EPD_2IN7_V2_WIDTH % 8 == 0)? (EPD_2IN7_V2_WIDTH / 8 ): (EPD_2IN7_V2_WIDTH / 8 + 1)) * EPD_2IN7_V2_HEIGHT;
const std::string PATH = "/home/strebdom/data/";
const std::string ETHLOGO = "/home/pi/git/Meteobike/Images/eth.bmp";

#endif