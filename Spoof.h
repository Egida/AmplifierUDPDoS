#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include "ServerFilter.h"

void insertDataPacket(char *const datagramBuff, const uint8_t *const packet, const size_t packetSz);
size_t buildSpoofPacket(char *const datagramBuff, const ServeraAmplf &serverAmplf,
                               const char *const targetHost, const uint16_t targetPort, const size_t packetSz);