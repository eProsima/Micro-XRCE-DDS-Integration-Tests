#include <gtest/gtest.h>

#include <Client.hpp>
#if defined(PLATFORM_NAME_LINUX)
#include <uxr/agent/transport/udp/UDPServerLinux.hpp>
#include <uxr/agent/transport/tcp/TCPServerLinux.hpp>
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
        , client_(0.0f, 8)
    {
        init_agent(AGENT_PORT);
    }

    ~ClientAgentInteraction() override
    {}

    void SetUp() override
    {
        ASSERT_NO_FATAL_FAILURE(client_.init_transport(transport_, "127.0.0.1", AGENT_PORT));
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
                agent_.reset(new eprosima::uxr::UDPv4Agent(port, std::get<1>(GetParam())));
                break;
            case TCP_TRANSPORT:
                agent_.reset(new eprosima::uxr::TCPv4Agent(port, std::get<1>(GetParam())));
                break;
        }
        agent_->run();
        agent_->set_verbose_level(6);
    }

protected:
    int transport_;
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
        case MiddlewareKind::NONE:
            std::exit(EXIT_FAILURE);
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
        case MiddlewareKind::NONE:
            std::exit(EXIT_FAILURE);
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
        case MiddlewareKind::NONE:
            std::exit(EXIT_FAILURE);
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
        case MiddlewareKind::NONE:
            std::exit(EXIT_FAILURE);
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
        case MiddlewareKind::NONE:
            std::exit(EXIT_FAILURE);
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
        case MiddlewareKind::NONE:
            std::exit(EXIT_FAILURE);
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
        case MiddlewareKind::NONE:
            std::exit(EXIT_FAILURE);
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
        case MiddlewareKind::NONE:
            std::exit(EXIT_FAILURE);
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
        case MiddlewareKind::NONE:
            std::exit(EXIT_FAILURE);
    }
}

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
