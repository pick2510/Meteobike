#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <memory>
#include <string>
#include <sstream>
#include "meteobike.h"
#include "consts.h"
#include "writer.h"
#include "gpspoller.h"
#include "utils.h"
#include "dhtpoller.h"
#include "measurement.h"
#include "EPD_2in7_V2.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"

using namespace std;

std::atomic<bool> term_signal = false;

int main(int argc, char *argv[])
{
	string ip{utils::getIP()};
	string hostname{utils::getHostname()};
	cout << ip << endl;
	cout << hostname << endl;
	auto image = startUp(hostname, ip);

	/*Paint_DrawString_EN(4,4,"Starting", &Font16, WHITE, BLACK);
	stringstream ss;
	ss << "This is " << utils::getHostname();
	Paint_DrawString_EN(4,40,ss.str().c_str(), &Font12, WHITE, BLACK);
	EPD_2IN7_V2_Display(BlackImage.get());*/

	/*writer myfile("/home/strebdom/data");
	gpspoller mygps("localhost");
	std::thread pol_t(&gpspoller::startPoll, &mygps, &term_signal);
	std::this_thread::sleep_for(1s);
	auto myvar = mygps.getLastData();
	for (int i = 0; i < 15; i++)
	{
		auto myvar = mygps.getLastData();
		cout << myvar.has_fix << " " << myvar.latitude << endl;
		std::this_thread::sleep_for(2s);
	}
	term_signal.store(true);
	pol_t.join();*/
	dhtdata_r mydht;
	mydht.temperature = 20;
	mydht.humdidity = 50;
	gpsdata_r mygqps;
	measurement mymeasurement(mygqps, mydht);
}

std::unique_ptr<UBYTE> startUp(const string &hostname, const string &ip)
{
	if (DEV_Module_Init() != 0)
	{
		std::cerr << "Error, cannot open activate GPIO";
		return std::unique_ptr<UBYTE>(nullptr);
	}
	EPD_2IN7_V2_Init();
	EPD_2IN7_V2_Clear();
	std::unique_ptr<UBYTE> Image(new UBYTE[Imagesize]);
	Paint_NewImage(Image.get(), EPD_2IN7_V2_WIDTH, EPD_2IN7_V2_HEIGHT, 90, WHITE);
	Paint_Clear(WHITE);
	GUI_ReadBmp(ETHLOGO.c_str(), 0, 30);
	EPD_2IN7_V2_Display(Image.get());
	DEV_Delay_ms(2000);
	Paint_NewImage(Image.get(), EPD_2IN7_V2_WIDTH, EPD_2IN7_V2_HEIGHT, 0, WHITE);
	Paint_Clear(WHITE);
	Paint_DrawString_EN(4, 4, "Starting", &Roboto14, WHITE, BLACK);
	stringstream ss;
	ss << hostname;
	Paint_DrawString_EN(4, 40, ss.str().c_str(), &Roboto12, WHITE, BLACK);
	ss.str("");
	ss << ip;
	Paint_DrawString_EN(4, 60, ss.str().c_str(), &Roboto12, WHITE, BLACK);
	EPD_2IN7_V2_Display(Image.get());
	/*Paint_ClearWindows(4, 60, 4 + Roboto12.Width * ss.str().length(), 60 + Roboto12.Height, WHITE);
	Paint_DrawString_EN(4, 60, "sdfsdfsdf", &Roboto12, WHITE, BLACK);
	for (auto i = 0; i < 10; i++)
	{
		EPD_2IN7_V2_Display_Partial(Image.get());
		DEV_Delay_ms(1000);
	}*/
	return Image;
}