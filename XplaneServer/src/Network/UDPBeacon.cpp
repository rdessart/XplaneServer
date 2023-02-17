#include "UDPBeacon.h"

UDPBeaon::UDPBeaon()
{
    IPInfo ip;
    ip.str_broadcast = "127.0.0.255";
    ip.str_ip = "127.0.0.1";
    ip.str_subnet = "255.255.255.0";
    m_ip = ip;
}

UDPBeaon::UDPBeaon(IPInfo ip)
{
    _ips = FindIp();
    m_ip = ip;
}

int UDPBeaon::Initalize()
{
    
#ifdef IBM
    if (WSAStartup(MAKEWORD(2, 2), &_wsa))
    {
        m_logger.Log("[XPLMServer] Unable to initlaize WSA\n");
        return 0x01;
    }
#endif
    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (!ISVALIDSOCKET(_socket))
    {
        m_logger.Log("[XPLMServer] Socket is invalid\n");
        return 0x02;
    }
#ifdef IBM
    BOOL bOptVal = TRUE;
    int bOptLen = sizeof(BOOL);
    int res = setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOptVal, bOptLen);
    if (res == SOCKET_ERROR)
    {
        m_logger.Log("[XPLMServer]Unable to set socket as broadcast\n");
        return 0x03;
    }

#else
    int broadcast = 1;
    setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);
#endif
    return 0x00;
}

int UDPBeaon::SendMessage(json message)
{
    std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    message.emplace("Time", std::ctime(&end_time));
    message.emplace("IPAddress", this->GetIPAddress().str_ip);
    message.emplace("ListeningPort", 50555);
    message.emplace("EmitPort", 50556);
    //struct addrinfo hints;
    struct sockaddr_in ipTarget;
    int res = inet_pton(AF_INET, GetIPAddress().str_broadcast.c_str(), &ipTarget.sin_addr.s_addr);
    ipTarget.sin_port = htons(50888);
    ipTarget.sin_family = AF_INET;

    return sendto(_socket,
        message.dump().c_str(), 
        static_cast<int>(message.dump().length()),
        0, (sockaddr*)&ipTarget,
        static_cast<int>(sizeof(ipTarget)));
}

json UDPBeaon::ReceiveMessage()
{
    return json();
}

void UDPBeaon::SetIPAddress(IPInfo ip)
{
    gLock.lock();
        m_ip = ip;
    gLock.unlock();
}

IPInfo UDPBeaon::GetIPAddress()
{
    gLock.lock();
        IPInfo ip = m_ip;
    gLock.unlock();
    return ip;
}
