#ifndef SERVERCHECKER_H
#define SERVERCHECKER_H

#include <list>
#include <memory>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

struct ServeraAmplf
{
    std::string host;
    uint16_t port;
    bool operator==(const ServeraAmplf &s)
    {
        return (s.host == this->host && s.port == this->port);
    }
};

double filterServers(std::list<ServeraAmplf> &serverList, const double minCof, const uint8_t *packet, unsigned int packetSz);

#endif