#include <gtest/gtest.h>

#include <Client.hpp>
#if defined(PLATFORM_NAME_LINUX)
#include <uxr/agent/transport/udp/UDPServerLinux.hpp>
#include <uxr/agent/transport/tcp/TCPServerLinux.hpp>
#include <uxr/agent/transport/serial/SerialServerLinux.hpp>
#include <uxr/agent/transport/serial/baud_rate_table_linux.h>
#include <termio.h>
#include <fcntl.h>
#elif defined(PLATFORM_NAME_WINDOWS)
#include <uxr/agent/transport/udp/UDPServerWindows.hpp>
#include <uxr/agent/transport/tcp/TCPServerWindows.hpp>
#endif

#include <thread>

class PublisherSubscriberInteraction : public ::testing::TestWithParam<std::tuple<int, float, MiddlewareKind>>
{
public:
    const uint16_t AGENT_PORT = 2018;

    PublisherSubscriberInteraction()
    : transport_(std::get<0>(GetParam()))
    , fd_{-1}
    , middleware_{}
    , publisher_(std::get<1>(GetParam()), 8)
    , subscriber_(std::get<1>(GetParam()), 8)
    {
        switch (std::get<2>(GetParam()))
        {
            case MiddlewareKind::FAST:
                middleware_ = eprosima::uxr::Middleware::Kind::FAST;
                break;
            case MiddlewareKind::CED:
                middleware_ = eprosima::uxr::Middleware::Kind::CED;
                break;
        }
        init_agent(AGENT_PORT);
    }

    ~PublisherSubscriberInteraction() override
    {}

    void SetUp() override
    {
        switch(transport_)
        {
            case UDP_TRANSPORT:
            {
                UDPTransportInfo transport_info;
                transport_info.ip = "127.0.0.1";
                transport_info.port = AGENT_PORT;
                ASSERT_NO_FATAL_FAILURE(publisher_.init_transport<UDPTransportInfo>(transport_info));
                ASSERT_NO_FATAL_FAILURE(subscriber_.init_transport<UDPTransportInfo>(transport_info));
                break;
            }
            case TCP_TRANSPORT:
            {
                TCPTransportInfo transport_info;
                transport_info.ip = "127.0.0.1";
                transport_info.port = AGENT_PORT;
                ASSERT_NO_FATAL_FAILURE(publisher_.init_transport<TCPTransportInfo>(transport_info));
                ASSERT_NO_FATAL_FAILURE(subscriber_.init_transport<TCPTransportInfo>(transport_info));
                break;
            }
#ifndef _WIN32
            case SERIAL_TRANSPORT:
            {
                SerialTransportInfo transport_info;
                transport_info.dev = ptsname(fd_);
                transport_info.remote_addr = 0;
                transport_info.local_addr = 1;
                ASSERT_NO_FATAL_FAILURE(publisher_.init_transport<SerialTransportInfo>(transport_info));
                transport_info.local_addr = 2;
                ASSERT_NO_FATAL_FAILURE(subscriber_.init_transport<SerialTransportInfo>(transport_info));
                break;
            }
#endif
        }

        switch (std::get<2>(GetParam()))
        {
            case MiddlewareKind::FAST:
                ASSERT_NO_FATAL_FAILURE(publisher_.create_entities_xml<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK, 0));
                ASSERT_NO_FATAL_FAILURE(subscriber_.create_entities_xml<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK, 0));
                break;
            case MiddlewareKind::CED:
                ASSERT_NO_FATAL_FAILURE(publisher_.create_entities_xml<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK, 0));
                ASSERT_NO_FATAL_FAILURE(subscriber_.create_entities_xml<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK, 0));
                break;
        }
    }

    void TearDown() override
    {
        ASSERT_NO_FATAL_FAILURE(publisher_.close_transport(transport_));
        ASSERT_NO_FATAL_FAILURE(subscriber_.close_transport(transport_));
    }

    void init_agent(uint16_t port)
    {
        switch(transport_)
        {
            case UDP_TRANSPORT:
                agent_.reset(new eprosima::uxr::UDPv4Agent(port, middleware_));
                break;
            case TCP_TRANSPORT:
                agent_.reset(new eprosima::uxr::TCPv4Agent(port, middleware_));
                break;
#ifndef _WIN32
            case SERIAL_TRANSPORT:
                char* dev = nullptr;
                fd_ = posix_openpt(O_RDWR | O_NOCTTY);
                if (-1 != fd_)
                {
                    if (grantpt(fd_) == 0 && unlockpt(fd_) == 0 && (dev = ptsname(fd_)))
                    {
                        fcntl(fd_, F_SETPIPE_SZ, 4096);

                        struct termios attr;
                        tcgetattr(fd_, &attr);
                        cfmakeraw(&attr);
                        tcflush(fd_, TCIOFLUSH);
                        tcsetattr(fd_, TCSANOW, &attr);

                        /* Get baudrate. */
                        speed_t baudrate = getBaudRate("115200");

                        /* Setting BAUD RATE. */
                        cfsetispeed(&attr, baudrate);
                        cfsetospeed(&attr, baudrate);

                        /* Log. */
                        std::cout << "Pseudo-Serial device opend at " << dev << std::endl;

                        /* Run server. */
                        agent_.reset(new eprosima::uxr::SerialAgent(fd_, 0x00, middleware_));
                    }
                }
                break;
#endif
        }
        agent_->run();
        agent_->set_verbose_level(6);
    }

    void check_messages(std::string message, size_t number, uint8_t stream_id_raw)
    {
        std::thread publisher_thread(&Client::publish, &publisher_, 1, stream_id_raw, number, message);
        std::thread subscriber_thread(&Client::subscribe, &subscriber_, 1, stream_id_raw, number, message);

        publisher_thread.join();
        subscriber_thread.join();
    }

protected:
    int transport_;
    int fd_;
    eprosima::uxr::Middleware::Kind middleware_;
    std::unique_ptr<eprosima::uxr::Server> agent_;
    Client publisher_;
    Client subscriber_;
    static const std::string SMALL_MESSAGE;
};

const std::string PublisherSubscriberInteraction::SMALL_MESSAGE("Hello DDS world!");

#ifndef _WIN32
INSTANTIATE_TEST_CASE_P(
        TransportAndLost,
        PublisherSubscriberInteraction,
        ::testing::Combine(
            ::testing::Values(UDP_TRANSPORT, TCP_TRANSPORT, SERIAL_TRANSPORT),
            ::testing::Values(0.0f, 0.05f, 0.1f),
            ::testing::Values(MiddlewareKind::FAST, MiddlewareKind::CED)));
#else
INSTANTIATE_TEST_CASE_P(
        TransportAndLost,
        PublisherSubscriberInteraction,
        ::testing::Combine(
            ::testing::Values(UDP_TRANSPORT, TCP_TRANSPORT),
            ::testing::Values(0.0f, 0.05f, 0.1f),
            ::testing::Values(MiddlewareKind::FAST, MiddlewareKind::CED)));
#endif

TEST_P(PublisherSubscriberInteraction, PubSub1TopicsBestEffort)
{
    if(0.0f == std::get<1>(GetParam())) //only without lost
    {
        check_messages(SMALL_MESSAGE, 1, 0x01);
    }
}

TEST_P(PublisherSubscriberInteraction, PubSub10TopicsBestEffort)
{
    if(0.0f == std::get<1>(GetParam())) //only without lost
    {
        check_messages(SMALL_MESSAGE, 10, 0x01);
    }
}

TEST_P(PublisherSubscriberInteraction, PubSub1TopicsReliable)
{
    check_messages(SMALL_MESSAGE, 1, 0x80);
}

TEST_P(PublisherSubscriberInteraction, PubSub10TopicsReliable)
{
    check_messages(SMALL_MESSAGE, 10, 0x80);
}

// TODO (#4423) Fix the non-reliable behavior when messages is higher than the agent history to enable this
/*TEST_P(PublisherSubscriberInteraction, PubSub30TopicsReliable)
{
    check_messages(SMALL_MESSAGE, 30, 0x80);
}
*/

TEST_P(PublisherSubscriberInteraction, PubSub1FragmentedTopic2Parts)
{
    std::string message(size_t(publisher_.get_mtu() * 1.5), 'A');
    check_messages(message, 1, 0x80);
}

TEST_P(PublisherSubscriberInteraction, PubSub3FragmentedTopic2Parts)
{
    std::string message(size_t(publisher_.get_mtu() * 1.5), 'A');
    check_messages(message, 3, 0x80);
}

TEST_P(PublisherSubscriberInteraction, PubSub1FragmentedTopic4Parts)
{
    std::string message(size_t(publisher_.get_mtu() * 3.5), 'A');
    check_messages(message, 1, 0x80);
}

TEST_P(PublisherSubscriberInteraction, PubSub3FragmentedTopic4Parts)
{
    std::string message(size_t(publisher_.get_mtu() * 3.5), 'A');
    check_messages(message, 3, 0x80);
}

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
