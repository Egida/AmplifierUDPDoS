#include "spoof.h"

#include <list>

void flood(const std::list <ServeraAmplf> &serverAmplificatorList,
           const char *targetHost,
           uint16_t targetPort,
           const uint8_t *packet,
           size_t packetSz,
           uint32_t sleep
);
