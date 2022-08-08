#include "ServerFilter.h"

static constexpr const unsigned long long timeoutRecvMs = 100;

double getCofServer(const ServeraAmplf &server, const double minCof, const uint8_t *packet, unsigned int packetSz)
{
    // Creating socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
        throw std::runtime_error("socket creation failed");

    timeval tv{};
    tv.tv_sec = 0;
    tv.tv_usec = 1000 * timeoutRecvMs;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv) < 0)
        throw std::runtime_error("error set recv timeout");

    // Filling server information
    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server.port);
    servaddr.sin_addr.s_addr = inet_addr(server.host.c_str());

    // Send
    const ssize_t sendSz = sendto(sockfd, packet, packetSz, 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    if (sendSz != packetSz)
    {
        close(sockfd);
        throw std::runtime_error("sent less");
    }

    // Recv
    ssize_t recvFull = 0;
    ssize_t recvBytes = 0;
    char buffer[1024];
    while ((recvBytes = recv(sockfd, buffer, sizeof(buffer), 0)) > 0)
        recvFull += recvBytes;

    close(sockfd);

    if (recvFull == 0)
        return -1; // Server not response

    // Calc
    return ((double)recvFull) / ((double)sendSz);
}

double filterServers(std::list<ServeraAmplf> &serverList, double minCof, const uint8_t *packet, const unsigned int packetSz)
{
    if (packet == nullptr)
        throw std::runtime_error("packet buffer is null!");

    if (packetSz == 0)
        throw std::runtime_error("packet buffer size is 0");

    if (serverList.empty())
        throw std::runtime_error("server list is empty");

    double fullAmplificationCofValue = 0;

    for (auto i = serverList.begin(); i != serverList.end();)
    {
        const auto &server = *i;
        try
        {
            const auto cof = getCofServer(server, minCof, packet, packetSz);
            if (cof < minCof)
            {
                serverList.erase(i++);
                if (cof == -1)
                    std::cout << "\t- Server not response " << server.host << ":" << server.port << std::endl;
                else
                    std::cout << "\t- Low gain [" << cof << "] " << server.host << ":" << server.port << std::endl;
                continue;
            }

            std::cout << "\t+ Amplification [" << cof << "] " << server.host << ":" << server.port << std::endl;
            fullAmplificationCofValue += cof;
            ++i;
        }
        catch (const std::runtime_error &exception)
        {
            std::cerr << exception.what() << ". Bypass server" << std::endl;
            serverList.erase(i++);
            continue;
        }
    }

    return fullAmplificationCofValue;
}