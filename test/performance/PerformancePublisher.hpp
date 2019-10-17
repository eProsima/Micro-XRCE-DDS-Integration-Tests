#ifndef IN_TEST_PERFORMANCEPUBLISHER_HPP
#define IN_TEST_PERFORMANCEPUBLISHER_HPP

#include "PerformanceClient.hpp"
#include "PerformanceTopic.hpp"
#include <EntitiesInfo.hpp>

#include <thread>

template<MiddlewareKind MK>
class PerformancePublisher : public PerformanceClient
{
public:
    PerformancePublisher() {}

    ~PerformancePublisher() override = default;

    template<size_t Size, typename D>
    void publish(
            D duration);

private:
    bool create_entities() final;

private:
    static uint16_t entities_prefix_;
};

template<MiddlewareKind MK>
template<size_t Size, typename D>
inline void PerformancePublisher<MK>::publish(
        D duration)
{
    uxrStreamId output_stream_id = uxr_stream_id_from_raw(0x01, UXR_OUTPUT_STREAM);
    uxrObjectId datawriter_id = uxr_object_id(entities_prefix_, UXR_DATAWRITER_ID);

    D elapsed_time{};
    std::chrono::time_point<std::chrono::high_resolution_clock> init_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> current_time;

    ucdrBuffer ub;
    PerformanceTopic<Size> topic = {0};

    init_time = std::chrono::high_resolution_clock::now();
    while (elapsed_time.count() < duration.count())
    {
        std::chrono::nanoseconds epoch_time = std::chrono::high_resolution_clock::now().time_since_epoch();
        topic.timestamp[0] = epoch_time.count() >> 32;
        topic.timestamp[1] = epoch_time.count() & UINT32_MAX;

        if (uxr_prepare_output_stream(&session_, output_stream_id, datawriter_id, &ub, Size) && topic.serialize(ub))
        {
            (void) uxr_run_session_time(&session_, 0);
        }

        current_time = std::chrono::high_resolution_clock::now();
        elapsed_time = std::chrono::duration_cast<D>(current_time - init_time);
    }
}

template<MiddlewareKind MK>
inline bool PerformancePublisher<MK>::create_entities()
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

    uxrObjectId publisher_id = uxr_object_id(entities_prefix_, UXR_PUBLISHER_ID);
    request_id = uxr_buffer_create_publisher_xml(
        &session_, output_stream_id, publisher_id, participant_id, EInfo::publisher_xml, flags);
    uxr_run_session_until_all_status(&session_, 3000, &request_id, &status, 1);
    if ((UXR_STATUS_OK != status) || (last_object_id_ != publisher_id) || (last_request_id_ != request_id))
    {
        return false;
    }

    uxrObjectId datawriter_id = uxr_object_id(entities_prefix_, UXR_DATAWRITER_ID);
    request_id = uxr_buffer_create_datawriter_xml(
        &session_, output_stream_id, datawriter_id, publisher_id, EInfo::datawriter_xml, flags);
    uxr_run_session_until_all_status(&session_, 3000, &request_id, &status, 1);
    if ((UXR_STATUS_OK != status) || (last_object_id_ != datawriter_id) || (last_request_id_ != request_id))
    {
        return false;
    }

    return true;
}

template<MiddlewareKind MK>
uint16_t PerformancePublisher<MK>::entities_prefix_ = 0x0000;

#endif // IN_TEST_PERFORMANCEPUBLISHER_HPP
