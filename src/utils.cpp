#include "utils.h"
#include "pi_2_dht_read.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <limits.h>
#include <cstring>
#include <iostream>

namespace utils
{

	std::string getIP()
	{
		int sock = socket(PF_INET, SOCK_DGRAM, 0);
		sockaddr_in loopback;

		if (sock == -1)
		{
			std::cerr << "Could not socket\n";
			return std::string("None");
		}

		memset(&loopback, 0, sizeof(loopback));
		loopback.sin_family = AF_INET;
		loopback.sin_addr.s_addr = 1337; // can be any IP address
		loopback.sin_port = htons(9);	 // using debug port

		if (connect(sock, reinterpret_cast<sockaddr *>(&loopback), sizeof(loopback)) == -1)
		{
			close(sock);
			std::cerr << "Could not connect\n";
			return std::string("None");
		}

		socklen_t addrlen = sizeof(loopback);
		if (getsockname(sock, reinterpret_cast<sockaddr *>(&loopback), &addrlen) == -1)
		{
			close(sock);
			std::cerr << "Could not getsockname\n";
			return std::string("None");
		}

		close(sock);

		char buf[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &loopback.sin_addr, buf, INET_ADDRSTRLEN) == 0x0)
		{
			std::cerr << "Could not inet_ntop\n";
			return std::string("None");
		}
		return std::string(buf);
	}

	std::string getHostname()
	{
		char hname[HOST_NAME_MAX];
		int error = 0;
		if (gethostname(hname, HOST_NAME_MAX) == -1)
		{
			std::cout << "Error, cannot get hostname" << std::endl;
			exit(EXIT_FAILURE);
		}
		return std::string(hname);
	}
}