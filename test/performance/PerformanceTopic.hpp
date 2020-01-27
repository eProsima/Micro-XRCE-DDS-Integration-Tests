#ifndef IN_TEST_PERFORMANCETOPIC_HPP
#define IN_TEST_PERFORMANCETOPIC_HPP

#include <ucdr/microcdr.h>

template<size_t Size>
struct PerformanceTopic
{
    uint32_t timestamp[2];
    uint8_t data[Size - sizeof(timestamp)];

    bool serialize(
        ucdrBuffer& ub) const
    {
        (void) ucdr_serialize_array_uint32_t(&ub, timestamp, 2);
        (void) ucdr_serialize_array_uint8_t(&ub, data, sizeof(data));
        return !ub.error;
    }

    bool deserialize(
            ucdrBuffer& ub)
    {
        (void) ucdr_deserialize_array_uint32_t(&ub, timestamp, 2);
        (void) ucdr_deserialize_array_uint8_t(&ub, data, sizeof(data));
        return !ub.error;
    }
};

#endif // IN_TEST_PERFORMANCETOPIC_HPP
