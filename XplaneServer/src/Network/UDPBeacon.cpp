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
    return 0x00;
}

int UDPBeaon::SendMessage(json message)
{
    std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    message.emplace("Time", std::ctime(&end_time));
    struct addrinfo hints;
    memset(&hints, 0x00, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo* targetAddress;
    getaddrinfo("127.0.0.1", "50888", &hints, &targetAddress);
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
