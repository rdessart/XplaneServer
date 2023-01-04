#pragma once

#include "IServer.h"
#include "NetworkUtils.h"

#include "../Tools/Logger.h"

class UDPBeaon : public IServer
{
public:
    UDPBeaon();
    int Initalize();
    int SendMessage(json message);
    json ReceiveMessage();
protected:
    Logger m_logger = Logger("XPLMServer.log", "UDPBeacon", true);
};