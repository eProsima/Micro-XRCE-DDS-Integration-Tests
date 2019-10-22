#include "PerformancePublisher.hpp"
#include "PerformanceSubscriber.hpp"

#include <iostream>
#include <iomanip>

constexpr size_t sep_width = 30;

template<MiddlewareKind MK, size_t S, typename D>
void launch_test(
        PerformancePublisher<MK>& publisher,
        PerformanceSubscriber<MK>& subscriber,
        D duration,
        uint64_t throughput)
{
    uint64_t ns = uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
    if (0 == throughput * ns / (std::nano::den * 8 * S))
    {
        return;
    }

    std::thread publisher_thread(
            &PerformancePublisher<MK>:: template publish<S, D>,
            &publisher,
            duration,
            throughput);
    std::thread subscriber_thread(
            &PerformanceSubscriber<MK>:: template subscribe<S, D>,
            &subscriber,
            duration);

    subscriber_thread.join();
    publisher_thread.join();

    std::cout.setf(std::ios::fixed);
    std::cout << std::setprecision(0);
    std::cout << std::setw(sep_width) << S;
    std::cout << std::setw(sep_width) << publisher.get_throughput();
    std::cout << std::setw(sep_width) << subscriber.get_throughput();
    std::cout << std::setw(sep_width) << subscriber.get_latency_avg();
    std::cout << std::setw(sep_width) << subscriber.get_latency_std();
    std::cout << std::endl;
}

template<MiddlewareKind MK, size_t F, size_t ...R, typename D>
typename std::enable_if<sizeof...(R) == 0>::type
for_each_launch_test(
        PerformancePublisher<MK>& publisher,
        PerformanceSubscriber<MK>& subscriber,
        D duration,
        uint64_t throughput)
{
    launch_test<MK, F>(publisher, subscriber, duration, throughput);
}

template<MiddlewareKind MK, size_t F, size_t ...R, typename D>
typename std::enable_if<sizeof...(R) != 0>::type
for_each_launch_test(
        PerformancePublisher<MK>& publisher,
        PerformanceSubscriber<MK>& subscriber,
        D duration,
        uint64_t throughput)
{
    launch_test<MK, F>(publisher, subscriber, duration, throughput);
    for_each_launch_test<MK, R...>(publisher, subscriber, duration, throughput);
}

int main()
{
    constexpr MiddlewareKind clients_middleware_kind = MiddlewareKind::CED;

    UDPTransportInfo transport_info;
    transport_info.ip = "127.0.0.1";
    transport_info.port = 2000;

    PerformancePublisher<clients_middleware_kind> publisher;
    publisher.init<UDPTransportInfo>(transport_info);

    PerformanceSubscriber<clients_middleware_kind> subscriber;
    subscriber.init<UDPTransportInfo>(transport_info);

    uint64_t throughput[] = {
        100,
        1   * std::kilo::num,
        10  * std::kilo::num,
        100 * std::kilo::num,
        1   * std::mega::num,
        10  * std::mega::num,
        100 * std::mega::num,
        1   * std::giga::num};

    std::cout << std::setw(sep_width) << "message_size(B)";
    std::cout << std::setw(sep_width) << "throughput_pub(b/s)";
    std::cout << std::setw(sep_width) << "throughput_sub(b/s)";
    std::cout << std::setw(sep_width) << "latency(us)";
    std::cout << std::setw(sep_width) << "jitter(us)";
    std::cout << std::endl;

    for (auto t : throughput)
    {
        for_each_launch_test<clients_middleware_kind, 2<<3, 2<<4, 2<<5, 2<<6, 2<<7, 2<<8, 2<<9, 2<<10, 2<<11, 2<<12, 2<<13, 2<<14, 63000>
            (publisher, subscriber, std::chrono::seconds(1), t);
    }

    return 0;
}
