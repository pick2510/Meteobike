#include <iostream>
#include "meteobike.h"
#include "consts.h"
#include "writer.h"
#include "pi_2_dht_read.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

using namespace std;

int main(int argc, char *argv[])
{
	cout << getIP() << endl;
	float temp, humidity;
	int error_lev = pi_2_dht_read(DHT22,10,&humidity,&temp);
	cout << error_lev << endl;
	writer myfile("/home/strebdom/data");
}

string getIP()
{
	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	sockaddr_in loopback;

	if (sock == -1)
	{
		cerr << "Could not socket\n";
		return string("None");
	}

	memset(&loopback, 0, sizeof(loopback));
	loopback.sin_family = AF_INET;
	loopback.sin_addr.s_addr = 1337; // can be any IP address
	loopback.sin_port = htons(9);	 // using debug port

	if (connect(sock, reinterpret_cast<sockaddr *>(&loopback), sizeof(loopback)) == -1)
	{
		close(sock);
		std::cerr << "Could not connect\n";
		return string("None");
	}

	socklen_t addrlen = sizeof(loopback);
	if (getsockname(sock, reinterpret_cast<sockaddr *>(&loopback), &addrlen) == -1)
	{
		close(sock);
		cerr << "Could not getsockname\n";
		return string("None");
	}

	close(sock);

	char buf[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &loopback.sin_addr, buf, INET_ADDRSTRLEN) == 0x0)
	{
		cerr << "Could not inet_ntop\n";
		return string("None");
	}
	return string(buf);

}