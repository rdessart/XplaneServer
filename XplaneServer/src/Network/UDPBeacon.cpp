#include "UDPBeacon.h"

UDPBeaon::UDPBeaon()
{
    _ips = FindIp();
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
    message.emplace("IPAddress", FindIp()[0]);
    message.emplace("ListeningPort", 50555);
    message.emplace("EmitPort", 50556);
    struct addrinfo hints;
    struct sockaddr_in ipTest;
    if(_ips[0] != "127.0.0.1")
    {
        ipTest.sin_addr.s_addr = INADDR_BROADCAST;
    }
    else
    {
        sockaddr_in send_address = {};
	    int res = inet_pton(AF_INET, "127.0.0.1", &ipTest.sin_addr.s_addr);
    }
    ipTest.sin_port = htons(50888);
    ipTest.sin_family = AF_INET;

    // memset(&hints, 0x00, sizeof(hints));
    // hints.ai_family = AF_INET;
    // hints.ai_socktype = SOCK_DGRAM;
    // hints.ai_flags = AI_PASSIVE;
    
    // getaddrinfo("192.168.0.255", "50888", &hints, &targetAddress);
    return sendto(_socket,
        message.dump().c_str(), 
        static_cast<int>(message.dump().length()),
        0, (sockaddr*)&ipTest,
        static_cast<int>(sizeof(ipTest)));
}

json UDPBeaon::ReceiveMessage()
{
    return json();
}
