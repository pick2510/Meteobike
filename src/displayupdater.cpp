#include "displayupdater.h"
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <chrono>

using namespace std;

#define bigfont Roboto13
#define smallfont Font12

displayupdater::displayupdater(threadsafe_queue<results> &inqueue, const std::string &hostname, const std::string &ip, const writer &mywriter, const std::atomic<bool> &is_writing) : queue(inqueue), ip(ip), hostname(hostname), mywriter(mywriter), is_writing(is_writing)
{
    stringstream therm_ss;
    ifstream therm("/sys/class/thermal/thermal_zone0/temp");
    image = std::make_unique<UBYTE[]>(Imagesize);
    if (!therm.is_open())
    {
        cerr << "Couldn't open /sys/class/thermal/thermal_zone0/temp, exiting!";
        exit(EXIT_FAILURE);
    }
    therm_ss << therm.rdbuf();
    therm.close();
    auto i_therm = std::stof(therm_ss.str()) / 1000;
    EPD_2IN7_Clear();
    Paint_NewImage(image.get(), EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 0, WHITE);
    Paint_Clear(WHITE);
    Paint_DrawString_EN(4, 0, "T: NaN C", &bigfont, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 1 * (bigfont.Height), "RH: NaN[%]", &bigfont, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 2 * (bigfont.Height), "pv: NaN[kpa]", &bigfont, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 3 * (bigfont.Height), "Speed: NaN[m/s]", &bigfont, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 4 * (bigfont.Height), "Altitude: NaN[m]", &bigfont, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 5 * (bigfont.Height), "Date/time: NaN", &bigfont, WHITE, BLACK);
    Paint_DrawLine(4, 6 * (bigfont.Height), EPD_2IN7_WIDTH, 6 * (bigfont.Height), BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawString_EN(4, 0 + 6 * (bigfont.Height) + smallfont.Height, hostname.c_str(), &smallfont, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 6 * (bigfont.Height) + 2 * (smallfont.Height), ip.c_str(), &smallfont, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 6 * (bigfont.Height) + 3 * (smallfont.Height), fmt::format("Records: {}", mywriter.counter).c_str(), &smallfont, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 6 * (bigfont.Height) + 4 * (smallfont.Height), fmt::format("Recording: {}", is_writing).c_str(), &smallfont, WHITE, BLACK);
    Paint_DrawString_EN(4, 0 + 6 * (bigfont.Height) + 5 * (smallfont.Height), fmt::format("CPU Temp: {}[C]", i_therm).c_str(), &smallfont, WHITE, BLACK);
    EPD_2IN7_Display(image.get());
    DEV_Delay_ms(3000);
}

void displayupdater::startUpdating()
{
    for (;;)
    {
        results res;
        queue.wait_and_pop(res);
        if (res.is_ending)
        {
            break;
        }
        stringstream therm_ss;
        therm.open("/sys/class/thermal/thermal_zone0/temp");
        therm_ss << therm.rdbuf();
        auto i_therm = std::stof(therm_ss.str()) / 1000;
        therm.close();
        // EPD_2IN7_Clear();
        Paint_NewImage(image.get(), EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 0, WHITE);
        Paint_Clear(WHITE);
        Paint_DrawString_EN(4, 0, fmt::format("T: {}[C]", res.dht.temperature).c_str(), &bigfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 1 * (bigfont.Height), fmt::format("RH: {}[%]", res.dht.humdidity).c_str(), &bigfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 2 * (bigfont.Height), fmt::format("pv: {:.3}[kpa]", res.pv / 1000).c_str(), &bigfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 3 * (bigfont.Height), fmt::format("Speed: {:.2}[m/s]", res.gps.speed).c_str(), &bigfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 4 * (bigfont.Height), fmt::format("Altitude:{}[m]", res.gps.altitude).c_str(), &bigfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 5 * (bigfont.Height), "Date/time (UTC):", &bigfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 6 * (bigfont.Height), fmt::format("{:%d-%m-%y %H:%M:%S}", timespecTotime(res.gps.t_time)).c_str(), &bigfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 7 * (bigfont.Height), fmt::format("Lat: {:.5}", res.gps.latitude).c_str(), &bigfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 8 * (bigfont.Height), fmt::format("Lon: {:.5}", res.gps.longitude).c_str(), &bigfont, WHITE, BLACK);
        Paint_DrawLine(4, 9 * (bigfont.Height), EPD_2IN7_WIDTH, 9 * (bigfont.Height), BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        Paint_DrawString_EN(4, 0 + 9 * (bigfont.Height) + smallfont.Height, hostname.c_str(), &smallfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 9 * (bigfont.Height) + 2 * (smallfont.Height), ip.c_str(), &smallfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 9 * (bigfont.Height) + 3 * (smallfont.Height), fmt::format("GPS has fix?: {}", res.gps.has_fix).c_str(), &smallfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 9 * (bigfont.Height) + 4 * (smallfont.Height), fmt::format("Records: {}", mywriter.counter).c_str(), &smallfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 9 * (bigfont.Height) + 5 * (smallfont.Height), fmt::format("Recording: {}", is_writing).c_str(), &smallfont, WHITE, BLACK);
        Paint_DrawString_EN(4, 0 + 9 * (bigfont.Height) + 6 * (smallfont.Height), fmt::format("CPU Temp: {}[C]", i_therm).c_str(), &smallfont, WHITE, BLACK);
        EPD_2IN7_Display(image.get());
    }
}

std::chrono::system_clock::time_point displayupdater::timespecTotime(timespec_t ts)
{

    auto duration = std::chrono::seconds{ts.tv_sec} + std::chrono::nanoseconds{ts.tv_nsec};

    auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    return std::chrono::system_clock::time_point(dur);
}

displayupdater::~displayupdater()
{
}
