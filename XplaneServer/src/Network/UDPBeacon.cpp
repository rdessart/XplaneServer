#include "UDPBeacon.h"

UDPBeaon::UDPBeaon()
{
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
    message.emplace("Emitter", FindIp()[0]);
    message.emplace("ListeningPort", 50555);
    message.emplace("EmitPort", 50556);
    struct addrinfo hints;
    memset(&hints, 0x00, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo* targetAddress;
    getaddrinfo("192.168.0.255", "50888", &hints, &targetAddress);
    return sendto(_socket,
        message.dump().c_str(), 
        static_cast<int>(message.dump().length()),
        0, targetAddress->ai_addr,
        static_cast<int>(targetAddress->ai_addrlen));
}

json UDPBeaon::ReceiveMessage()
{
    return json();
}
