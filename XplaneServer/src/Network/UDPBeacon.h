#pragma once

#include <mutex>

#include "IServer.h"
#include "NetworkUtils.h"

#include "../Tools/Logger.h"

class UDPBeaon : public IServer
{
public:
    UDPBeaon();
    UDPBeaon(IPInfo ip);
    int Initalize();
    int SendMessage(json message);
    json ReceiveMessage();
    void SetIPAddress(IPInfo ip);
    IPInfo GetIPAddress();
protected:
    Logger m_logger = Logger("XPLMServer.log", "UDPBeacon", true);
    std::vector<IPInfo> _ips;
    std::mutex gLock;
    IPInfo m_ip;
};