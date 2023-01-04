#pragma once

#include "IServer.h"
#include "../Tools/Logger.h"
#include "../Datarefs/DatarefManager.h"

class UDPServer
{
public:
	UDPServer();
    ~UDPServer();
    int Initalize();
    int SendMessage(json message);
    void ReceiveMessage(DatarefManager manager);
protected:
#ifdef IBM
    WSADATA _wsa = { 0 };
#endif
	SOCKET _socket = INVALID_SOCKET;
    Logger m_logger = Logger("XPLMServer.log", "UDPBeacon", true);
    struct addrinfo* _targetAddress = nullptr;
    fd_set master;
    SOCKET m_maxSocket = INVALID_SOCKET;
};

