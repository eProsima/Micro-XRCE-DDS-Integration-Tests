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

class ClientAgentInteraction : public ::testing::TestWithParam<std::tuple<int, MiddlewareKind>>
{
public:
    const uint16_t AGENT_PORT = 2018;
    const float LOST = 0.1f;

    ClientAgentInteraction()
        : transport_(std::get<0>(GetParam()))
        , fd_{-1}
        , middleware_{}
        , client_(0.0f, 8)
    {
        switch (std::get<1>(GetParam()))
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

    ~ClientAgentInteraction() override
    {}

    void SetUp() override
    {
        switch (transport_)
        {
            case UDP_TRANSPORT:
            {
                UDPTransportInfo transport_info;
                transport_info.ip = "127.0.0.1";
                transport_info.port = AGENT_PORT;
                ASSERT_NO_FATAL_FAILURE(client_.init_transport<UDPTransportInfo>(transport_info));
                break;
            }
            case TCP_TRANSPORT:
            {
                TCPTransportInfo transport_info;
                transport_info.ip = "127.0.0.1";
                transport_info.port = AGENT_PORT;
                ASSERT_NO_FATAL_FAILURE(client_.init_transport<TCPTransportInfo>(transport_info));
                break;
            }
            case SERIAL_TRANSPORT:
            {
                SerialTransportInfo transport_info;
                transport_info.dev = ptsname(fd_);
                transport_info.remote_addr = 0;
                transport_info.local_addr = 0;
                ASSERT_NO_FATAL_FAILURE(client_.init_transport<SerialTransportInfo>(transport_info));
                break;
            }
        }
    }

    void TearDown() override
    {
        ASSERT_NO_FATAL_FAILURE(client_.close_transport(transport_));
    }

    // TODO (#4334): Add serial tests.
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

protected:
    int transport_;
    int fd_;
    eprosima::uxr::Middleware::Kind middleware_;
    std::unique_ptr<eprosima::uxr::Server> agent_;
    Client client_;
};

INSTANTIATE_TEST_CASE_P(
        Transport,
        ClientAgentInteraction,
        ::testing::Combine(
            ::testing::Values(UDP_TRANSPORT, TCP_TRANSPORT),
            ::testing::Values(MiddlewareKind::FAST, MiddlewareKind::CED)));
//        ::testing::PrintToStringParamName());

TEST_P(ClientAgentInteraction, InitCloseSession)
{
    //Default behavior
}

TEST_P(ClientAgentInteraction, NewEntitiesCreationXMLBestEffort)
{
    switch (std::get<1>(GetParam()))
    {
        case MiddlewareKind::FAST:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::FAST>(1, 0x01, UXR_STATUS_OK, 0));
            break;
        case MiddlewareKind::CED:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::CED>(1, 0x01, UXR_STATUS_OK, 0));
            break;
    }
}

TEST_P(ClientAgentInteraction, NewEntitiesCreationXMLReliable)
{
    switch (std::get<1>(GetParam()))
    {
        case MiddlewareKind::FAST:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK, 0));
            break;
        case MiddlewareKind::CED:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK, 0));
            break;
    }
}

TEST_P(ClientAgentInteraction, NewEntitiesCreationREFBestEffort)
{
    switch (std::get<1>(GetParam()))
    {
        case MiddlewareKind::FAST:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_ref<MiddlewareKind::FAST>(1, 0x01, UXR_STATUS_OK, 0));
            break;
        case MiddlewareKind::CED:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_ref<MiddlewareKind::CED>(1, 0x01, UXR_STATUS_OK, 0));
            break;
    }
}

TEST_P(ClientAgentInteraction, NewEntitiesCreationREFReliable)
{
    switch (std::get<1>(GetParam()))
    {
        case MiddlewareKind::FAST:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_ref<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK, 0));
            break;
        case MiddlewareKind::CED:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_ref<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK, 0));
            break;
    }
}

TEST_P(ClientAgentInteraction, ExistantEntitiesCreationReuseXMLXMLReliable)
{
    switch (std::get<1>(GetParam()))
    {
        case MiddlewareKind::FAST:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK, 0));
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK_MATCHED, UXR_REUSE));
            break;
        case MiddlewareKind::CED:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK, 0));
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK_MATCHED, UXR_REUSE));
            break;
    }
}

/* TODO (#3589): Fix XML and REF reference issue to enable this test.
TEST_P(ClientAgentInteraction, ExistantEntitiesCreationReuseXMLREFReliable)
{
    ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));
    ASSERT_NO_FATAL_FAILURE(client_.create_entities_ref(1, 0x80, UXR_STATUS_OK_MATCHED, UXR_REUSE));
}
*/

TEST_P(ClientAgentInteraction, ExistantEntitiesCreationReuseREFREFReliable)
{
    switch (std::get<1>(GetParam()))
    {
        case MiddlewareKind::FAST:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_ref<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK, 0));
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_ref<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK_MATCHED, UXR_REUSE));
            break;
        case MiddlewareKind::CED:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_ref<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK, 0));
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_ref<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK_MATCHED, UXR_REUSE));
            break;
    }
}

TEST_P(ClientAgentInteraction, ExistantEntitiesCreationReplaceReliable)
{
    switch (std::get<1>(GetParam()))
    {
        case MiddlewareKind::FAST:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK, 0));
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK, UXR_REPLACE));
            break;
        case MiddlewareKind::CED:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK, 0));
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK, UXR_REPLACE));
            break;
    }
}

TEST_P(ClientAgentInteraction, ExistantEntitiesCreationNoReplaceReliable)
{
    switch (std::get<1>(GetParam()))
    {
        case MiddlewareKind::FAST:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK, 0));
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_ERR_ALREADY_EXISTS, 0));
            break;
        case MiddlewareKind::CED:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK, 0));
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_ERR_ALREADY_EXISTS, 0));
            break;
    }
}

TEST_P(ClientAgentInteraction, ExistantEntitiesCreationReplaceReuseReliable)
{
    switch (std::get<1>(GetParam()))
    {
        case MiddlewareKind::FAST:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK, 0));
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::FAST>(1, 0x80, UXR_STATUS_OK_MATCHED, UXR_REPLACE | UXR_REUSE));
            break;
        case MiddlewareKind::CED:
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK, 0));
            ASSERT_NO_FATAL_FAILURE(client_.create_entities_xml<MiddlewareKind::CED>(1, 0x80, UXR_STATUS_OK_MATCHED, UXR_REPLACE | UXR_REUSE));
            break;
    }
}

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
