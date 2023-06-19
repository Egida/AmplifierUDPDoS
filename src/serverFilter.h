#ifndef SERVERCHECKER_H
#define SERVERCHECKER_H

#include <list>
#include <string>
#include <memory>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

struct ServeraAmplf
{
    std::string host{};
    uint16_t port{};
    bool operator==(const ServeraAmplf &s) const noexcept
    {
        return (s.host == this->host && s.port == this->port);
    }
};

double filterServers(std::list<ServeraAmplf> &serverList, double minCof, const uint8_t *packet, size_t packetSz);

#endif
