#include "displayupdater.h"
#include <fmt/format.h>

using namespace std;



displayupdater::displayupdater(threadsafe_queue<results> &inqueue, const std::string &hostname, const std::string &ip, const writer &mywriter, const std::atomic<bool> &is_writing) : queue(inqueue), ip(ip), hostname(hostname), mywriter(mywriter), is_writing(is_writing)
{
    stringstream therm_ss;
    ifstream therm("/sys/class/thermal/thermal_zone0/temp");
    UBYTE *image = new UBYTE[Imagesize];
    if (!therm.is_open())
    {
        cerr << "Couldn't open /sys/class/thermal/thermal_zone0/temp, exiting!";
        exit(EXIT_FAILURE);
    }
    therm_ss << therm.rdbuf();
    auto i_therm = std::stof(therm_ss.str()) / 1000;
    therm_ss.seekg(0);
    EPD_2IN7_Clear();
    Paint_NewImage(image, EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 0, WHITE);
    Paint_Clear(WHITE);
    Paint_DrawString_EN(4, 0, "T: NaN C", &Roboto13, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 1 * (Roboto13.Height), "RH: NaN%", &Roboto13, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 2 * (Roboto13.Height), "pv: NaN kpa", &Roboto13, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 3 * (Roboto13.Height), "Speed: NaN m/s", &Roboto13, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 4 * (Roboto13.Height), "Altitude: m", &Roboto13, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 5 * (Roboto13.Height), "Date/time: NaN", &Roboto13, WHITE, BLACK);
    Paint_DrawLine(4, 6 * (Roboto13.Height), EPD_2IN7_WIDTH, 6 * (Roboto13.Height), BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawString_EN(4, 0 + 6 * (Roboto13.Height) + Roboto12.Height, hostname.c_str(), &Roboto12, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 6 * (Roboto13.Height) + 2 * (Roboto12.Height), ip.c_str(), &Roboto12, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 6 * (Roboto13.Height) + 3 * (Roboto12.Height), fmt::format("Records: {}", mywriter.counter).c_str(), &Roboto12, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 6 * (Roboto13.Height) + 4 * (Roboto12.Height), fmt::format("Recording: {}", is_writing).c_str(), &Roboto12, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 6 * (Roboto13.Height) + 5 * (Roboto12.Height), fmt::format("CPU Temp: {} C", i_therm).c_str(), &Roboto12, WHITE, BLACK);
    EPD_2IN7_Display(image);
    delete image;
}

void displayupdater::startUpdating(std::atomic<bool> *signal)
{
    for (;;)
    {
        results res;
        queue.wait_and_pop(res);
        /*stringstream therm_ss;
        therm.open("/sys/class/thermal/thermal_zone0/temp");
        therm_ss << therm.rdbuf();
        auto i_therm = std::stof(therm_ss.str()) / 1000;
        therm_ss.seekg(0);*/
        EPD_2IN7_Clear();
        image = new UBYTE[Imagesize];
        Paint_NewImage(image, EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 0, WHITE);
        Paint_Clear(WHITE);
        Paint_DrawString_EN(4, 0, fmt::format("T: {} C",res.dht.temperature).c_str(), &Roboto13, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 1 * (Roboto13.Height), fmt::format("RH: {}%", res.dht.humdidity).c_str(), &Roboto13, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 2 * (Roboto13.Height), fmt::format("pv: {:2f} kpa",res.pv).c_str(), &Roboto13, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 3 * (Roboto13.Height), fmt::format("Speed: {} m/s",res.gps.speed).c_str(), &Roboto13, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 4 * (Roboto13.Height), fmt::format("Altitude:{} m",res.gps.altitude).c_str(), &Roboto13, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 5 * (Roboto13.Height), "Date/time:", &Roboto13, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 6 * (Roboto13.Height), res.gps.time.c_str(), &Roboto13, WHITE, BLACK);
        Paint_DrawLine(4, 8 * (Roboto13.Height), EPD_2IN7_WIDTH, 8 * (Roboto13.Height), BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        Paint_DrawString_EN(4, 0 + 8 * (Roboto13.Height) + Roboto12.Height, hostname.c_str(), &Roboto12, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 8 * (Roboto13.Height) + 2 * (Roboto12.Height), ip.c_str(), &Roboto12, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 8 * (Roboto13.Height) + 3 * (Roboto12.Height), fmt::format("Records: {}", mywriter.counter).c_str(), &Roboto12, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 8 * (Roboto13.Height) + 4 * (Roboto12.Height), fmt::format("Recording: {}", is_writing).c_str(), &Roboto12, WHITE, BLACK);
        //Paint_DrawString_EN(4, 0 + 8 * (Roboto13.Height) + 5 * (Roboto12.Height), fmt::format("CPU Temp: {} C", i_therm).c_str(), &Roboto12, WHITE, BLACK);
        EPD_2IN7_Display(image);
        delete image;
    }
}

displayupdater::~displayupdater()
{
    delete image;
}
