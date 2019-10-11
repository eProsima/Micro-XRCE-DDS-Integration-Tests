#include <TransportInfo.hpp>

#include <uxr/client/client.h>

#include <memory>

#define PERFORMANCE_HISTORY 16

class PerformanceClient
{
public:
    PerformanceClient()
        : client_key_{++next_client_key_}
        , transport_kind_{0}
    {}

    virtual ~PerformanceClient() = default;

    template<typename T>
    bool init(
            const T& transport_info);

    bool fini();

    virtual bool create_entities() = 0;

private:
    bool init_common(
        size_t mtu);

    void setup_streams(
        size_t mtu);

    static void topic_callback_dispatcher(
            uxrSession* session,
            uxrObjectId object_id,
            uint16_t request_id,
            uxrStreamId stream_id,
            ucdrBuffer* serialization,
            void* args);

    static void status_callback_dispatcher(
            uxrSession* session,
            uxrObjectId object_id,
            uint16_t request_id,
            uint8_t status,
            void* args);

    virtual void topic_callback(
            uxrSession* session_,
            uxrObjectId object_id,
            uint16_t request_id,
            uxrStreamId stream_id,
            ucdrBuffer* serialization) = 0;

    virtual void status_callback(
            uxrSession* session,
            uxrObjectId object_id,
            uint16_t request_id,
            uint8_t status) = 0;

protected:
    uxrSession session_;

private:
    static uint32_t next_client_key_;
    uint32_t client_key_;

    uint8_t transport_kind_;
    uxrUDPTransport udp_transport_;
    uxrUDPPlatform udp_platform_;

    std::unique_ptr<uint8_t[]> output_best_effort_stream_buffer_;
    std::unique_ptr<uint8_t[]> output_reliable_stream_buffer_;
    std::unique_ptr<uint8_t[]> input_reliable_stream_buffer_;
};

template<>
inline bool PerformanceClient::init<UDPTransportInfo>(
        const UDPTransportInfo& transport_info)
{
    bool rv = false;
    transport_kind_ = UDP_TRANSPORT;
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

inline bool PerformanceClient::fini()
{
    bool rv = false;
    if (0 != transport_kind_ && uxr_delete_session(&session_))
    {
        switch (transport_kind_)
        {
            case UDP_TRANSPORT:
                rv = uxr_close_udp_transport(&udp_transport_);
                break;
        }
    }
    return rv;
}

inline bool PerformanceClient::init_common(
        size_t mtu)
{
    uxr_set_topic_callback(&session_, topic_callback_dispatcher, this);
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
    for(size_t i = 0; i < UXR_CONFIG_MAX_OUTPUT_BEST_EFFORT_STREAMS; ++i)
    {
        uint8_t* buffer = output_best_effort_stream_buffer_.get() + mtu * i;
        (void) uxr_create_output_best_effort_stream(&session_, buffer, mtu);
    }
    for(size_t i = 0; i < UXR_CONFIG_MAX_INPUT_BEST_EFFORT_STREAMS; ++i)
    {
        (void) uxr_create_input_best_effort_stream(&session_);
    }
    for(size_t i = 0; i < UXR_CONFIG_MAX_OUTPUT_RELIABLE_STREAMS; ++i)
    {
        uint8_t* buffer = output_reliable_stream_buffer_.get() + mtu * PERFORMANCE_HISTORY * i;
        (void) uxr_create_output_reliable_stream(&session_, buffer , mtu * PERFORMANCE_HISTORY, PERFORMANCE_HISTORY);
    }
    for(size_t i = 0; i < UXR_CONFIG_MAX_INPUT_RELIABLE_STREAMS; ++i)
    {
        uint8_t* buffer = input_reliable_stream_buffer_.get() + mtu * PERFORMANCE_HISTORY * i;
        (void) uxr_create_input_reliable_stream(&session_, buffer, mtu * PERFORMANCE_HISTORY, PERFORMANCE_HISTORY);
    }
}

void PerformanceClient::topic_callback_dispatcher(
        uxrSession* session,
        uxrObjectId object_id,
        uint16_t request_id,
        uxrStreamId stream_id,
        ucdrBuffer* serialization,
        void* args)
{
    static_cast<PerformanceClient*>(args)->topic_callback(session, object_id, request_id, stream_id, serialization);
}

void PerformanceClient::status_callback_dispatcher(
        uxrSession* session,
        uxrObjectId object_id,
        uint16_t request_id,
        uint8_t status,
        void* args)
{
    static_cast<PerformanceClient*>(args)->status_callback(session, object_id, request_id, status);
}
