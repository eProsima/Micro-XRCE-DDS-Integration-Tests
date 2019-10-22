#include "PerformanceTest.hpp"

int main()
{
    MiddlewareKind mk = MiddlewareKind::FAST;
    TransportKind transport_kind = TransportKind::tcp;
    std::chrono::seconds duration{1};

    switch (transport_kind)
    {
        case TransportKind::udp:
        {
            UDPTransportInfo transport_info;
            transport_info.ip = "127.0.0.1";
            transport_info.port = 2000;
            run_test(mk, transport_info, duration);
            break;
        }
        case TransportKind::tcp:
        {
            TCPTransportInfo transport_info;
            transport_info.ip = "127.0.0.1";
            transport_info.port = 2000;
            run_test(mk, transport_info, duration);
            break;
        }
        default:
            break;
    }

    return 0;
}
