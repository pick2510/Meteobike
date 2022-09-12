#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include "meteobike.h"
#include "ts_queue.h"
#include "consts.h"
#include "writer.h"
#include "gpspoller.h"
#include "utils.h"
#include "dhtpoller.h"
#include "measurement.h"
//#include "EPD_2in7_V2.h"
#include "EPD_2in7.h"
#include "bcm2835.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"

using namespace std;

std::atomic<bool> term_signal = false;

int main(int argc, char *argv[])
{
	if (getuid() != 0){
		cerr << "Program must be run as root. Exiting" << endl;
		exit(EXIT_FAILURE);
	}
	ifstream therm("/sys/class/thermal/thermal_zone0/temp");
	if (!therm.is_open()){
		cerr << "Couldn't open /sys/class/thermal/thermal_zone0/temp, exiting!";
		exit(EXIT_FAILURE);
	}
	bool writeRecords = true;
	string ip{utils::getIP()};
	string hostname{utils::getHostname()};
	cout << ip << endl;
	cout << hostname << endl;
	setupGPIO();
	if(auto image = startUp(hostname, ip, therm) == nullptr)
	{
		cerr << "Error, couldn't allocate array for for display" << endl;
		exit(EXIT_FAILURE);
	}
	dhtpoller mydht(PIN);
	gpspoller mygps("localhost");
	writer output("/home/pi/data/", hostname, ip);
	std::thread dht_t(&dhtpoller::startPoll, &mydht, &term_signal);
	std::thread gps_t(&gpspoller::startPoll, &mygps, &term_signal);
	threadsafe_queue<results> tq();
	
	for (;;)
	{
		auto dhtdata = mydht.getLatestData();
		auto gpsdata = mygps.getLastData();
		measurement mymeas(gpsdata, dhtdata);
		cout << "Humidity: " << dhtdata.humdidity << " Temperature: " << dhtdata.temperature << endl;
		cout << "#############################" << endl;
		cout << "GPS has fix?: " << gpsdata.has_fix << " Time: " << gpsdata.time << " GPS lat: " << gpsdata.latitude << " GPS lon: " << gpsdata.longitude << " Alt: " << gpsdata.altitude << endl;
		auto results = mymeas.retres();
		if (writeRecords)
		{
			output.createRecord(results);
			output.writeRecord();
		}
		auto event = checkEvent();
		if (event != KEYS::NOKEY)
		{
			parseEvent(event, writeRecords);
		}
		if (term_signal.load())
			break;
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}

	dht_t.join();
	gps_t.join();
	return EXIT_SUCCESS;
}

std::unique_ptr<UBYTE> startUp(const string &hostname, const string &ip, ifstream &therm)
{
	stringstream therm_ss;
	therm_ss << therm.rdbuf();
	auto i_therm = std::stof(therm_ss.str())/1000;
	therm_ss.seekg(0);
	if (DEV_Module_Init() != 0)
	{
		std::cerr << "Error, cannot open activate GPIO";
		return std::unique_ptr<UBYTE>(nullptr);
	}
	EPD_2IN7_Init();
	EPD_2IN7_Clear();
	std::unique_ptr<UBYTE> Image(new UBYTE[Imagesize]);
	Paint_NewImage(Image.get(), EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 90, WHITE);
	Paint_Clear(WHITE);
	GUI_ReadBmp(ETHLOGO.c_str(), 0, 30);
	EPD_2IN7_Display(Image.get());
	DEV_Delay_ms(2000);
	// EPD_2IN7_Clear();
	Paint_NewImage(Image.get(), EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 0, WHITE);
	Paint_Clear(WHITE);
	Paint_DrawString_EN(4, 4, "Starting", &Roboto14, WHITE, BLACK);
	stringstream ss;
	ss << hostname;
	Paint_DrawString_EN(4, 40, ss.str().c_str(), &Roboto12, WHITE, BLACK);
	ss.str("");
	ss << ip;
	Paint_DrawString_EN(4, 60, ss.str().c_str(), &Roboto12, WHITE, BLACK);
	Paint_DrawString_EN(4, EPD_2IN7_HEIGHT - (2 * Roboto12.Height), "Key 1: Pause/Resume", &Roboto12, WHITE, BLACK);
	Paint_DrawString_EN(4, EPD_2IN7_HEIGHT - (1 * Roboto12.Height), "Key 4: Exit", &Roboto12, WHITE, BLACK);
	EPD_2IN7_Display(Image.get());
	DEV_Delay_ms(3000);
	return Image;
}

void setupGPIO()
{
	bcm2835_init();
	bcm2835_gpio_fsel(KEYS::KEY1, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(KEYS::KEY2, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(KEYS::KEY3, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(KEYS::KEY4, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(KEYS::KEY1, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_set_pud(KEYS::KEY2, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_set_pud(KEYS::KEY3, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_set_pud(KEYS::KEY4, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_len(KEYS::KEY1);
	bcm2835_gpio_len(KEYS::KEY2);
	bcm2835_gpio_len(KEYS::KEY3);
	bcm2835_gpio_len(KEYS::KEY4);
}

void parseEvent(const KEYS &key, bool &writeRecord)
{
	switch (key)
	{
	case KEYS::KEY1:
	{
		cout << "Pause!" << endl;
		writeRecord = !writeRecord;
		if (writeRecord)
			cout << "write enablde" << endl;
		else
			cout << "write disabled" << endl;
		break;
	}
	case KEYS::KEY4:
	{
		cout << "exit!" << endl;
		term_signal.store(true);
		break;
	}

	default:
		break;
	}
}

KEYS checkEvent()
{
	if (bcm2835_gpio_eds(KEYS::KEY1))
	{
		// Now clear the eds flag by setting it to 1
		bcm2835_gpio_set_eds(KEYS::KEY1);
		cout << "Event Key 1 detected" << endl;
		return KEYS::KEY1;
	}
	if (bcm2835_gpio_eds(KEYS::KEY2))
	{
		// Now clear the eds flag by setting it to 1
		bcm2835_gpio_set_eds(KEYS::KEY2);
		cout << "Event Key 2 detected" << endl;
		return KEYS::KEY2;
	}
	if (bcm2835_gpio_eds(KEYS::KEY3))
	{
		// Now clear the eds flag by setting it to 1
		bcm2835_gpio_set_eds(KEYS::KEY3);
		cout << "Event Key 3 detected" << endl;
		return KEYS::KEY3;
	}
	if (bcm2835_gpio_eds(KEYS::KEY4))
	{
		// Now clear the eds flag by setting it to 1
		bcm2835_gpio_set_eds(KEYS::KEY4);
		cout << "Event Key 4 detected" << endl;
		return KEYS::KEY4;
	}
	return KEYS::NOKEY;
}