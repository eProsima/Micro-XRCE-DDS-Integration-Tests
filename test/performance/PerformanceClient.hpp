#ifndef IN_TEST_PERFORMANCECLIENT_HPP
#define IN_TEST_PERFORMANCECLIENT_HPP

#include <TransportInfo.hpp>

#include <uxr/client/client.h>

#include <memory>
#include <chrono>
#ifndef _WIN32
#include <stdio.h>
#include <fcntl.h>
#endif // _WIN32

#define PERFORMANCE_HISTORY 16

inline bool operator == (const uxrObjectId& lhs, const uxrObjectId& rhs)
{
    return (lhs.id == rhs.id) && (lhs.type == rhs.type);
}

inline bool operator != (const uxrObjectId& lhs, const uxrObjectId& rhs)
{
    return !(lhs == rhs);
}

class PerformanceClient
{
public:
    PerformanceClient()
        : client_key_{++next_client_key_}
        , transport_kind_{TransportKind::none}
    {}

    virtual ~PerformanceClient() = default;

    template<typename T>
    bool init(
            const T& transport_info);

    bool fini();

private:
    virtual bool create_entities() = 0;

    bool init_common(
            size_t mtu);

    void setup_streams(
            size_t mtu);

    static void status_callback_dispatcher(
            uxrSession* session,
            uxrObjectId object_id,
            uint16_t request_id,
            uint8_t status,
            void* args);

    virtual void status_callback(
            uxrSession* session,
            uxrObjectId object_id,
            uint16_t request_id,
            uint8_t status);

protected:
    uxrSession session_;

    uint8_t last_status_;
    uxrObjectId last_object_id_;
    uint16_t last_request_id_;

private:
    static uint32_t next_client_key_;
    uint32_t client_key_;

    TransportKind transport_kind_;
    uxrUDPTransport udp_transport_;
    uxrUDPPlatform udp_platform_;
    uxrTCPTransport tcp_transport_;
    uxrTCPPlatform tcp_platform_;
    uxrSerialTransport serial_transport_;
    uxrSerialPlatform serial_platform_;

    std::unique_ptr<uint8_t[]> output_best_effort_stream_buffer_;
    std::unique_ptr<uint8_t[]> output_reliable_stream_buffer_;
    std::unique_ptr<uint8_t[]> input_reliable_stream_buffer_;
};

template<>
inline bool PerformanceClient::init<UDPTransportInfo>(
        const UDPTransportInfo& transport_info)
{
    bool rv = false;
    transport_kind_ = TransportKind::udp;
    if (uxr_init_udp_transport(&udp_transport_, &udp_platform_, transport_info.ip, transport_info.port))
    {
        uxr_init_session(&session_, &udp_transport_.comm, client_key_);
        if (init_common(UXR_CONFIG_UDP_TRANSPORT_MTU))
        {
            rv = create_entities();
        }
    }
    return rv;
}

template<>
inline bool PerformanceClient::init<TCPTransportInfo>(
        const TCPTransportInfo& transport_info)
{
    bool rv = false;
    transport_kind_ = TransportKind::tcp;
    if (uxr_init_tcp_transport(&tcp_transport_, &tcp_platform_, transport_info.ip, transport_info.port))
    {
        uxr_init_session(&session_, &tcp_transport_.comm, client_key_);
        if (init_common(UXR_CONFIG_TCP_TRANSPORT_MTU))
        {
            rv = create_entities();
        }
    }
    return rv;
}

#ifndef _WIN32
template<>
inline bool PerformanceClient::init<SerialTransportInfo>(
        const SerialTransportInfo& transport_info)
{
    bool rv = false;
    transport_kind_ = TransportKind::serial;
    int fd = open(transport_info.dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (uxr_init_serial_transport(&serial_transport_, &serial_platform_, fd, transport_info.remote_addr, transport_info.local_addr))
    {
        uxr_init_session(&session_, &serial_transport_.comm, client_key_);
        if (init_common(UXR_CONFIG_SERIAL_TRANSPORT_MTU))
        {
            rv = create_entities();
        }
    }
    return rv;
}
#endif // _WIN32

inline bool PerformanceClient::fini()
{
    bool rv = false;
    if (TransportKind::none != transport_kind_ && uxr_delete_session(&session_))
    {
        switch (transport_kind_)
        {
            case TransportKind::none:
                break;
            case TransportKind::udp:
                rv = uxr_close_udp_transport(&udp_transport_);
                break;
            case TransportKind::tcp:
                rv = uxr_close_tcp_transport(&tcp_transport_);
                break;
#ifndef _WIN32
            case TransportKind::serial:
                rv = uxr_close_serial_transport(&serial_transport_);
                break;
#endif // _WIN32
        }
    }
    return rv;
}

inline bool PerformanceClient::init_common(
        size_t mtu)
{
    uxr_set_status_callback(&session_, status_callback_dispatcher, this);
    setup_streams(mtu);
    return uxr_create_session(&session_);
}

inline void PerformanceClient::setup_streams(
        size_t mtu)
{
    output_best_effort_stream_buffer_.reset(new uint8_t[mtu * UXR_CONFIG_MAX_OUTPUT_BEST_EFFORT_STREAMS]{0});
    output_reliable_stream_buffer_.reset(new uint8_t[mtu * PERFORMANCE_HISTORY * UXR_CONFIG_MAX_OUTPUT_RELIABLE_STREAMS]{0});
    input_reliable_stream_buffer_.reset(new uint8_t[mtu * PERFORMANCE_HISTORY * UXR_CONFIG_MAX_INPUT_RELIABLE_STREAMS]{0});
    for(size_t i = 0; i < 1; ++i)
    {
        uint8_t* buffer = output_best_effort_stream_buffer_.get() + mtu * i;
        (void) uxr_create_output_best_effort_stream(&session_, buffer, mtu);
    }
    for(size_t i = 0; i < 1; ++i)
    {
        (void) uxr_create_input_best_effort_stream(&session_);
    }
    for(size_t i = 0; i < 1; ++i)
    {
        uint8_t* buffer = output_reliable_stream_buffer_.get() + mtu * PERFORMANCE_HISTORY * i;
        (void) uxr_create_output_reliable_stream(&session_, buffer , mtu * PERFORMANCE_HISTORY, PERFORMANCE_HISTORY);
    }
    for(size_t i = 0; i < 1; ++i)
    {
        uint8_t* buffer = input_reliable_stream_buffer_.get() + mtu * PERFORMANCE_HISTORY * i;
        (void) uxr_create_input_reliable_stream(&session_, buffer, mtu * PERFORMANCE_HISTORY, PERFORMANCE_HISTORY);
    }
}

inline void PerformanceClient::status_callback_dispatcher(
        uxrSession* session,
        uxrObjectId object_id,
        uint16_t request_id,
        uint8_t status,
        void* args)
{
    static_cast<PerformanceClient*>(args)->status_callback(session, object_id, request_id, status);
}

inline void PerformanceClient::status_callback(
        uxrSession *session,
        uxrObjectId object_id,
        uint16_t request_id,
        uint8_t status)
{
    (void) session;
    last_status_ = status;
    last_object_id_ = object_id;
    last_request_id_ = request_id;
    return;
}

uint32_t PerformanceClient::next_client_key_ = 0;

#endif // IN_TEST_PERFORMANCECLIENT_HPP
