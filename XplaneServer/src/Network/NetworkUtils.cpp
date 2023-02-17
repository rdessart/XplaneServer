#include "NetworkUtils.h"


std::vector<IPInfo> FindIp()
{
    std::vector<IPInfo> ips;
#ifndef IBM
    struct ifaddrs* addrs, * ifloop;
    char buf[64];
    struct sockaddr_in* s4;
    getifaddrs(&addrs);
    in_addr_t loopback = inet_addr("127.0.0.1");
    for (ifloop = addrs; ifloop != NULL; ifloop = ifloop->ifa_next)
    {
        if (ifloop->ifa_addr->sa_family != AF_INET) continue;
        s4 = (struct sockaddr_in*)(ifloop->ifa_addr);
        if (s4->sin_addr.s_addr == loopback) continue;
        inet_ntop(ifloop->ifa_addr->sa_family, (void*)&(s4->sin_addr), buf, sizeof(buf))/* == NULL*/;
        ips.push_back(std::string(buf));
    }
    freeifaddrs(addrs);
#else
    ULONG buflen = sizeof(IP_ADAPTER_INFO);
    auto pAdapterInfo = static_cast<IP_ADAPTER_INFO*>(malloc(buflen));

    if (GetAdaptersInfo(pAdapterInfo, &buflen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = static_cast<IP_ADAPTER_INFO*>(malloc(buflen));
    }

    if (GetAdaptersInfo(pAdapterInfo, &buflen) == NO_ERROR) {
        for (IP_ADAPTER_INFO* pAdapter = pAdapterInfo; pAdapter; pAdapter = pAdapter->Next)
        {
            if (strcmp(pAdapter->IpAddressList.IpAddress.String, "0.0.0.0") != 0)
            {
                IPInfo ipInfo;
                ipInfo.str_ip = pAdapter->IpAddressList.IpAddress.String;
                ipInfo.str_subnet = pAdapter->IpAddressList.IpMask.String;

                unsigned int ipr, sur;
                inet_pton(AF_INET, ipInfo.str_ip.c_str(), &ipr);
                inet_pton(AF_INET, ipInfo.str_subnet.c_str(), &sur);
                unsigned int broadcast = ((ipr & sur) | ~sur);
                char buffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &broadcast, buffer, INET_ADDRSTRLEN);
                ipInfo.str_broadcast = std::string(buffer);
                ips.push_back(ipInfo);
            }
        }
    }
    if (pAdapterInfo) free(pAdapterInfo);
#endif
    // if no  adapter we still work on localhost.
    if (ips.empty())
    {
        IPInfo ip;
        ip.str_ip = "127.0.0.1";
        ip.str_subnet = "255.255.255.0";
        ip.str_broadcast = "255.255.255.0";
        ips.push_back(ip);
    }
    return ips;
}
