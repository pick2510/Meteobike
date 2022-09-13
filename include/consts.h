#ifndef _CONSTS
#define _CONSTS


#include <string>
#include <iostream>
#include "EPD_2in7.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"


enum KEYS {NOKEY=0, KEY1=5, KEY2=6, KEY3=13, KEY4=19};


constexpr int REFRESH = 5;
constexpr int PIN = 4;
constexpr int DHT_SLEEP = 3;
constexpr int GPS_SLEEP = 1;
constexpr int UPDATE_EVERY_POINT = 5;
constexpr int MAXTIMINGS=100;
constexpr auto kWaitingTime{1000000}; 
constexpr UWORD Imagesize = ((EPD_2IN7_WIDTH % 8 == 0)? (EPD_2IN7_WIDTH / 8 ): (EPD_2IN7_WIDTH / 8 + 1)) * EPD_2IN7_HEIGHT;
const std::string PATH = "/home/strebdom/data/";
const std::string ETHLOGO = "/home/pi/git/Meteobike/Images/eth.bmp";

#endif