#include <nlohmann/json.hpp>

#ifndef IBM
	#include <netinet/in.h>
#else
    #include <WinSock2.h>
	#include <WS2tcpip.h>
#endif

using json = nlohmann::json;

struct Message
{
    json message;
    struct sockaddr_storage target;
    socklen_t target_lenght;
};