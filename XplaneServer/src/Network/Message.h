#ifndef IBM
	#include <netinet/in.h>
#else
    #include <WinSock2.h>
	#include <WS2tcpip.h>
#endif
#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct Message
{
    json message;
    struct sockaddr_storage target;
    socklen_t target_lenght;
};