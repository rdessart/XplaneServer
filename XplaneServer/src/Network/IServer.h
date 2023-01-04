#pragma once

#ifndef IBM
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <errno.h>

	#define ISVALIDSOCKET(s) ((s) >= 0)
	#define CLOSESOCKET(s) close(s)
	#define SOCKET int
	#define GETSOCKETERRNO() (errno)
	#define INVALID_SOCKET 0
#else
	#define _WINSOCKAPI_
	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT 0x0600
	#endif
	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#pragma comment(lib, "ws2_32.lib")

	#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
	#define CLOSESOCKET(s) closesocket(s)
	#define GETSOCKETERRNO() (WSAGetLastError())
#endif

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class IServer {
public:
	virtual int SendMessage(json message) = 0;
	virtual json ReceiveMessage() = 0;
	virtual int Initalize() = 0;
protected:
#ifdef IBM
	WSADATA _wsa = { 0 };
#endif
	SOCKET _socket = INVALID_SOCKET;
};