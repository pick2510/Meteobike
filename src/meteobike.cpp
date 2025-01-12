#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include "meteobike.h"
#include <fmt/format.h>
#include "unistd.h"
#include "displayupdater.h"
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
using namespace std::chrono;

std::atomic<bool> term_signal = false;

int main(int argc, char *argv[])
{
	auto myargs = getOptions(argc, argv);
	cout << "Log every " << myargs.log_every_second << " second" << endl;
	cout << "Display update interval " << myargs.display_update_interval << " seconds" << endl;
	if (getuid() != 0)
	{
		cerr << "Program must be run as root. Exiting" << endl;
		exit(EXIT_FAILURE);
	}
	int internal_counter = 0;
	std::atomic<bool> writeRecords = true;
	string ip{utils::getIP()};
	string hostname{utils::getHostname()};
	cout << ip << endl;
	cout << hostname << endl;
	setupGPIO();
	startUp(hostname, ip, myargs);
	dhtpoller mydht(PIN);
	gpspoller mygps("localhost");
	writer output("/home/pi/data/", hostname, ip);
	threadsafe_queue<results_r> tq;
	displayupdater mydsp(tq, hostname, ip, output, writeRecords);
	std::thread dht_t(&dhtpoller::startPoll, &mydht, &term_signal);
	std::thread gps_t(&gpspoller::startPoll, &mygps, &term_signal);
	std::thread display_t(&displayupdater::startUpdating, &mydsp);
	auto t1{steady_clock::now()};
	for (;;)
	{
		auto dhtdata = mydht.getLatestData();
		auto gpsdata = mygps.getLastData();
		measurement mymeas(gpsdata, dhtdata);
		cout << "Humidity: " << dhtdata.humdidity << " Temperature: " << dhtdata.temperature << endl;
		cout << "#############################" << endl;
		cout << "GPS has fix?: " << gpsdata.has_fix << " Time: " << gpsdata.time << " GPS lat: " << gpsdata.latitude << " GPS lon: " << gpsdata.longitude << " Alt: " << gpsdata.altitude << " Speed: " << gpsdata.speed << endl;
		auto results = mymeas.retres();
		auto t2{steady_clock::now()};
		if (duration_cast<seconds>(t2 - t1).count() >= myargs.display_update_interval)
		{
			tq.push(results);
			t1 = steady_clock::now();
		}
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
		{
			results.is_ending = true;
			tq.push(results);
			display_t.join();
			break;
		}
		std::this_thread::sleep_for(std::chrono::seconds(myargs.log_every_second));
		internal_counter++;
	}
	dht_t.join();
	gps_t.join();
	return EXIT_SUCCESS;
}

void startUp(const string &hostname, const string &ip, const args &myargs)
{

	if (DEV_Module_Init() != 0)
	{
		std::cerr << "Error, cannot open activate GPIO";
		exit(EXIT_FAILURE);
	}
	EPD_2IN7_Init();
	EPD_2IN7_Clear();
	auto Image = std::make_unique<UBYTE[]>(Imagesize);
	Paint_NewImage(Image.get(), EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 0, WHITE);
	Paint_Clear(WHITE);
	GUI_ReadBmp(ETHLOGO.c_str(), 0, 0);
	EPD_2IN7_Display(Image.get());
	DEV_Delay_ms(3000);
	// EPD_2IN7_Clear();
	Paint_NewImage(Image.get(), EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 0, WHITE);
	Paint_Clear(WHITE);
	Paint_DrawString_EN(4, 0, "Starting", &Roboto14, WHITE, BLACK);
	Paint_DrawString_EN(4, 0 + Roboto14.Height, fmt::format("Interval Logging: {} s", myargs.log_every_second).c_str(), &Font12, WHITE, BLACK);
	Paint_DrawString_EN(4, 0 + Roboto14.Height + Font12.Height, fmt::format("Interval Display: {} s", myargs.display_update_interval).c_str(), &Font12, WHITE, BLACK);
	stringstream ss;
	ss << hostname;
	Paint_DrawString_EN(4, 60, ss.str().c_str(), &Roboto12, WHITE, BLACK);
	ss.str("");
	ss << ip;
	Paint_DrawString_EN(4, 80, ss.str().c_str(), &Roboto12, WHITE, BLACK);
	Paint_DrawString_EN(4, EPD_2IN7_HEIGHT - (2 * Roboto12.Height), "Key 1: Pause/Resume", &Roboto12, WHITE, BLACK);
	Paint_DrawString_EN(4, EPD_2IN7_HEIGHT - (1 * Roboto12.Height), "Key 4: Exit", &Roboto12, WHITE, BLACK);
	EPD_2IN7_Display(Image.get());
	DEV_Delay_ms(4000);
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

void parseEvent(const KEYS &key, std::atomic<bool> &writeRecord)
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

void showHelp()
{
	cout << "Error: Please define parameter -d x -i y" << endl;
	cout << "-d [x] display update interval in seconds" << endl;
	cout << "-i [x] logging update intervals" << endl;
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

args getOptions(int &argc, char **argv)
{
	args myargs = {.log_every_second = -1, .display_update_interval = -1};
	int opt;
	while ((opt = getopt(argc, argv, "d:i:")) != -1)
	{
		switch (opt)
		{
		case 'd':
			myargs.display_update_interval = std::atoi(optarg);
			break;
		case 'i':
			myargs.log_every_second = std::atoi(optarg);
			break;
		default:
			showHelp();
			exit(EXIT_FAILURE);
		}
	}
	if (myargs.display_update_interval < 0 || myargs.log_every_second < 0)
	{
		showHelp();
		exit(EXIT_FAILURE);
	}
	return myargs;
}