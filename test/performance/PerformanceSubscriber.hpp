#ifndef IN_TEST_PERFORMANCESUBSCRIBER_HPP
#define IN_TEST_PERFORMANCESUBSCRIBER_HPP

#include "PerformanceClient.hpp"
#include "PerformanceTopic.hpp"
#include <EntitiesInfo.hpp>

#include <thread>
#include <iostream>

class PerformanceSubscriber : public PerformanceClient
{
public:
    PerformanceSubscriber() {}

    ~PerformanceSubscriber() override = default;

    template<size_t Size, typename D>
    void subscribe(
            D duration);

private:
    bool create_entities() final;

    void topic_callback(
            uxrSession* session,
            uxrObjectId object_id,
            uint16_t request_id,
            uxrStreamId stream_id,
            ucdrBuffer* serialization) final;

private:
    static uint16_t entities_prefix_;
};

template<size_t Size, typename D>
inline void PerformanceSubscriber::subscribe(
        D duration)
{
    uxrStreamId output_stream_id = uxr_stream_id(0, UXR_RELIABLE_STREAM, UXR_OUTPUT_STREAM);
    uxrStreamId input_stream_id = uxr_stream_id_from_raw(0x01, UXR_INPUT_STREAM);
    uxrObjectId datareader_id = uxr_object_id(entities_prefix_, UXR_DATAREADER_ID);

    uxrDeliveryControl delivery_control = {};
    delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
    uint16_t request_id = uxr_buffer_request_data(&session_, output_stream_id, datareader_id, input_stream_id, &delivery_control);

    D elapsed_time{};
    std::chrono::time_point<std::chrono::high_resolution_clock> init_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> current_time;

    init_time = std::chrono::high_resolution_clock::now();
    while (elapsed_time.count() < duration.count())
    {
        uxr_run_session_time(&session_, 1000);
        current_time = std::chrono::high_resolution_clock::now();
        elapsed_time = std::chrono::duration_cast<D>(current_time - init_time);
    }
}

inline bool PerformanceSubscriber::create_entities()
{
    using EInfo = EntitiesInfo<MiddlewareKind::FAST>;

    uint8_t flags = 0x00;
    uxrStreamId output_stream_id = uxr_stream_id_from_raw(0x01, UXR_OUTPUT_STREAM);
    uint16_t request_id; uint8_t status;

    uxrObjectId participant_id = uxr_object_id(entities_prefix_, UXR_PARTICIPANT_ID);
    request_id = uxr_buffer_create_participant_xml(
        &session_, output_stream_id, participant_id, 0, EInfo::participant_xml, flags);
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

inline void PerformanceSubscriber::topic_callback(
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

    PerformanceTopic<16> topic;
    topic.deserialize(*serialization);
    uint64_t timestamp = (uint64_t(topic.timestamp[0]) << 32) + topic.timestamp[1];

    std::chrono::nanoseconds epoch_time = std::chrono::high_resolution_clock::now().time_since_epoch();
    std::cout << epoch_time.count() - timestamp << std::endl;
}

uint16_t PerformanceSubscriber::entities_prefix_ = 0x0000;

#endif // IN_TEST_PERFORMANCESUBSCRIBER_HPP
