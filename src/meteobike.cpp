#include <iostream>
#include "meteobike.h"
#include "consts.h"
#include "writer.h"
#include "gpspoller.h"
#include <atomic>
#include <thread>
#include <chrono>
#include "utils.h"
#include "dhtpoller.h"

using namespace std;

std::atomic<bool> term_signal = false;

int main(int argc, char *argv[])
{
	//cout << utils::getIP() << endl;
	cout << utils::getHostname() << endl;
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
	dhtpoller mydht(11);
    std::thread dht_t(&dhtpoller::startPoll, &mydht, &term_signal);
    std::this_thread::sleep_for(std::chrono::seconds(1));	
	auto myvar = mydht.getLatestData();
	std::this_thread::sleep_for(10s);
	term_signal.store(true);
	dht_t.join();
	
}
