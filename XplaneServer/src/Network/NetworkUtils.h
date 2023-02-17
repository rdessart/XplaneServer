#pragma once

#ifdef IBM
	#define _WINSOCKAPI_
	#include <WinSock2.h>
	#include <iostream>
	#include <iphlpapi.h>
	#include <WS2tcpip.h>
	#pragma comment(lib, "IPHLPAPI.lib")
#else
	#include <unistd.h>
	#include <ifaddrs.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <iostream>
#endif

#include <fstream>
#include <string>
#include <vector>
#include <sstream>

struct IPInfo {
	std::string str_ip;
	std::string str_subnet;
	std::string str_broadcast;
};


std::vector<IPInfo> FindIp();

