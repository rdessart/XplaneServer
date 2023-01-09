#pragma once

#include <mutex>

#include "IServer.h"
#include "NetworkUtils.h"

#include "../Tools/Logger.h"

class UDPBeaon : public IServer
{
public:
    UDPBeaon();
    UDPBeaon(std::string ip);
    int Initalize();
    int SendMessage(json message);
    json ReceiveMessage();
    void SetIPAddress(std::string ip);
    std::string GetIPAddress();
protected:
    Logger m_logger = Logger("XPLMServer.log", "UDPBeacon", true);
    std::vector<std::string> _ips;
    std::mutex gLock;
    std::string m_ip;
};