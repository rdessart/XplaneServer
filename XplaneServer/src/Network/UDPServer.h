#pragma once

#include "IServer.h"
#include "../Tools/Logger.h"

class UDPServer : public IServer
{
public:
	UDPServer();
    ~UDPServer();
    int Initalize();
    int SendMessage(json message);
    json ReceiveMessage();
protected:
    Logger m_logger = Logger("XPLMServer.log", "UDPBeacon", true);
    struct addrinfo* _targetAddress = nullptr;
    fd_set master;
    SOCKET m_maxSocket = INVALID_SOCKET;
};

