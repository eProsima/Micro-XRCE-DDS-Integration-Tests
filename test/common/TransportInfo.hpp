#ifndef IN_TEST_TRANSPORTINFO_HPP
#define IN_TEST_TRANSPORTINFO_HPP

#include <stdint.h>

#define UDP_TRANSPORT 1
#define TCP_TRANSPORT 2
#define SERIAL_TRANSPORT 3

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
