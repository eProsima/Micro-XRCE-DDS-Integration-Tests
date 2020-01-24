#ifndef IN_TEST_TRANSPORTINFO_HPP
#define IN_TEST_TRANSPORTINFO_HPP

#include <stdint.h>

enum class TransportKind : uint8_t
{
    none,
    udp,
    tcp,
    serial
};

struct IPTransportInfo
{
    const char* ip;
    uint16_t port;
};

struct UDPTransportInfo : public IPTransportInfo {};

struct TCPTransportInfo : public IPTransportInfo {};

struct SerialTransportInfo
{
    const char* dev;
    uint8_t remote_addr;
    uint8_t local_addr;
};


#endif // IN_TEST_TRANSPORTINFO_HPP
