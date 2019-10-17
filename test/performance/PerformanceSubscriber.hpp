#ifndef IN_TEST_PERFORMANCESUBSCRIBER_HPP
#define IN_TEST_PERFORMANCESUBSCRIBER_HPP

#include "PerformanceClient.hpp"
#include "PerformanceTopic.hpp"
#include <EntitiesInfo.hpp>

#include <thread>
#include <cmath>

template<MiddlewareKind MK>
class PerformanceSubscriber : public PerformanceClient
{
public:
    PerformanceSubscriber() {}

    ~PerformanceSubscriber() override = default;

    template<size_t Size, typename D>
    void subscribe(
            D duration);

    double get_latency_avg() { return latency_avg_; }
    double get_latency_std() { return latency_std_; }
    double get_throughput() { return throughput_; }

private:
    bool create_entities() final;

    template<size_t Size>
    static void topic_callback_dispatcher(
            uxrSession* session,
            uxrObjectId object_id,
            uint16_t request_id,
            uxrStreamId stream_id,
            ucdrBuffer* serialization,
            void* args);

    template<size_t Size>
    void topic_callback(
            uxrSession* session,
            uxrObjectId object_id,
            uint16_t request_id,
            uxrStreamId stream_id,
            ucdrBuffer* serialization);

    void processing_latency(
            double current_latency);

    void init_subscription();

    template<typename D>
    void fini_subscription(
            D real_duration,
            size_t msg_size);

private:
    static uint16_t entities_prefix_;

    double latency_avg_;
    double latency_sum_;
    double latency_sum_2_;
    double latency_std_;
    double latency_ref_;
    double throughput_;
    uint64_t msg_count_;
};

template<MiddlewareKind MK>
template<size_t Size, typename D>
inline void PerformanceSubscriber<MK>::subscribe(
        D duration)
{
    init_subscription();

    uxr_set_topic_callback(&session_, topic_callback_dispatcher<Size>, this);

    uxrStreamId output_stream_id = uxr_stream_id(0, UXR_RELIABLE_STREAM, UXR_OUTPUT_STREAM);
    uxrStreamId input_stream_id = uxr_stream_id_from_raw(0x01, UXR_INPUT_STREAM);
    uxrObjectId datareader_id = uxr_object_id(entities_prefix_, UXR_DATAREADER_ID);

    uxrDeliveryControl delivery_control = {};
    delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
    uxr_buffer_request_data(&session_, output_stream_id, datareader_id, input_stream_id, &delivery_control);

    D elapsed_time{};
    std::chrono::time_point<std::chrono::high_resolution_clock> init_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> current_time;

    init_time = std::chrono::high_resolution_clock::now();
    while (elapsed_time.count() < duration.count())
    {
        uxr_run_session_until_timeout(&session_, 100);
        current_time = std::chrono::high_resolution_clock::now();
        elapsed_time = std::chrono::duration_cast<D>(current_time - init_time);
    }

    fini_subscription(elapsed_time, Size);
}

template<MiddlewareKind MK>
inline bool PerformanceSubscriber<MK>::create_entities()
{
    using EInfo = EntitiesInfo<MK>;

    uint8_t flags = 0x00;
    uxrStreamId output_stream_id = uxr_stream_id_from_raw(0x01, UXR_OUTPUT_STREAM);
    uint16_t request_id; uint8_t status;

    uxrObjectId participant_id = uxr_object_id(entities_prefix_, UXR_PARTICIPANT_ID);
    request_id = uxr_buffer_create_participant_xml(
        &session_, output_stream_id, participant_id, 11, EInfo::participant_xml, flags);
    uxr_run_session_until_all_status(&session_, 3000, &request_id, &status, 1);
    if ((UXR_STATUS_OK != status) || (last_object_id_ != participant_id) || (last_request_id_ != request_id))
    {
        return false;
    }

    uxrObjectId topic_id = uxr_object_id(entities_prefix_, UXR_TOPIC_ID);
    request_id = uxr_buffer_create_topic_xml(
        &session_, output_stream_id, topic_id, participant_id, EInfo::topic_xml, flags);
    uxr_run_session_until_all_status(&session_, 3000, &request_id, &status, 1);
    if ((UXR_STATUS_OK != status) || (last_object_id_ != topic_id) || (last_request_id_ != request_id))
    {
        return false;
    }

    uxrObjectId subscriber_id = uxr_object_id(entities_prefix_, UXR_SUBSCRIBER_ID);
    request_id = uxr_buffer_create_subscriber_xml(
        &session_, output_stream_id, subscriber_id, participant_id, EInfo::subscriber_xml, flags);
    uxr_run_session_until_all_status(&session_, 3000, &request_id, &status, 1);
    if ((UXR_STATUS_OK != status) || (last_object_id_ != subscriber_id) || (last_request_id_ != request_id))
    {
        return false;
    }

    uxrObjectId datareader_id = uxr_object_id(entities_prefix_, UXR_DATAREADER_ID);
    request_id = uxr_buffer_create_datareader_xml(
        &session_, output_stream_id, datareader_id, subscriber_id, EInfo::datareader_xml, flags);
    uxr_run_session_until_all_status(&session_, 3000, &request_id, &status, 1);
    if ((UXR_STATUS_OK != status) || (last_object_id_ != datareader_id) || (last_request_id_ != request_id))
    {
        return false;
    }

    return true;
}

template<MiddlewareKind MK>
template<size_t Size>
inline void PerformanceSubscriber<MK>::topic_callback_dispatcher(
        uxrSession* session,
        uxrObjectId object_id,
        uint16_t request_id,
        uxrStreamId stream_id,
        ucdrBuffer* serialization,
        void* args)
{
    static_cast<PerformanceSubscriber*>(args)->topic_callback<Size>(session, object_id, request_id, stream_id, serialization);
}

template<MiddlewareKind MK>
template<size_t Size>
inline void PerformanceSubscriber<MK>::topic_callback(
        uxrSession* session,
        uxrObjectId object_id,
        uint16_t request_id,
        uxrStreamId stream_id,
        ucdrBuffer* serialization)
{
    (void) session;
    (void) object_id;
    (void) request_id;
    (void) stream_id;

    PerformanceTopic<Size> topic;
    topic.deserialize(*serialization);
    uint64_t timestamp = (uint64_t(topic.timestamp[0]) << 32) + topic.timestamp[1];

    std::chrono::nanoseconds epoch_time = std::chrono::high_resolution_clock::now().time_since_epoch();

    ++msg_count_;
    processing_latency(epoch_time.count() - timestamp);
}

template<MiddlewareKind MK>
inline void PerformanceSubscriber<MK>::processing_latency(
        double current_latency)
{
    if (0.0 == current_latency)
    {
        latency_ref_ = current_latency;
        latency_avg_ = current_latency;
    }
    else
    {
        latency_avg_ += (current_latency - latency_avg_) / double(msg_count_);
        latency_sum_ += current_latency - latency_ref_;
        latency_sum_2_ += (current_latency - latency_ref_) * (current_latency - latency_ref_);
    }
}

template<MiddlewareKind MK>
inline void PerformanceSubscriber<MK>::init_subscription()
{
    latency_avg_ = 0;
    latency_sum_ = 0;
    latency_sum_2_ = 0;
    latency_std_ = 0;
    latency_ref_ = 0;
    msg_count_ = 0;
}

template<MiddlewareKind MK>
template<typename D>
inline void PerformanceSubscriber<MK>::fini_subscription(
        D real_duration,
        size_t msg_size)
{
    latency_std_ = std::sqrt((latency_sum_2_ - (latency_sum_ * latency_sum_) / msg_count_) / (msg_count_ - 1));
    throughput_ = double(msg_count_) / double(std::chrono::duration_cast<std::chrono::nanoseconds>(real_duration).count());
    throughput_ *= std::nano::den * msg_size * 8;
}

template<MiddlewareKind MK>
uint16_t PerformanceSubscriber<MK>::entities_prefix_ = 0x0000;

#endif // IN_TEST_PERFORMANCESUBSCRIBER_HPP
