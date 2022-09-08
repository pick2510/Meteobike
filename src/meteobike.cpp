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
	cout << utils::getIP() << endl;
	cout << utils::getHostname() << endl;
	DEV_Module_Init();
	EPD_2IN7_V2_Init();
	EPD_2IN7_V2_Clear();
	UWORD Imagesize = ((EPD_2IN7_V2_WIDTH % 8 == 0)? (EPD_2IN7_V2_WIDTH / 8 ): (EPD_2IN7_V2_WIDTH / 8 + 1)) * EPD_2IN7_V2_HEIGHT;
	std::unique_ptr<UBYTE> BlackImage (new UBYTE[Imagesize]);
	Paint_NewImage(BlackImage.get(), EPD_2IN7_V2_WIDTH, EPD_2IN7_V2_HEIGHT, 90, WHITE);
	Paint_Clear(WHITE);
	/*Paint_DrawString_EN(4,4,"Starting", &Font16, WHITE, BLACK);
	stringstream ss;
	ss << "This is " << utils::getHostname();
	Paint_DrawString_EN(4,40,ss.str().c_str(), &Font12, WHITE, BLACK);
	EPD_2IN7_V2_Display(BlackImage.get());*/
	GUI_ReadBmp("/home/pi/git/Meteobike/Images/eth.bmp", 0,30);
	EPD_2IN7_V2_Display(BlackImage.get());




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
	measurement mymeasurement(mygqps,mydht);

}
