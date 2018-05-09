#include <gtest/gtest.h>
#include <ClientInteraction.hpp>
#include <AgentInteraction.hpp>
#include <algorithm>
#include <thread>
#include <asio.hpp>
#include <functional>
#include <chrono>
#include <random>

class Gateway
{
public:
    Gateway(uint16_t fake_agent_port, uint16_t real_agent_port, float lost)
        : last_client_message_lost_(false)
        , last_agent_message_lost_(false)
        , client_socket_(io_service_, asio::ip::udp::endpoint(asio::ip::udp::v4(), fake_agent_port))
        , agent_socket_(io_service_)
        , agent_endpoint_(asio::ip::udp::endpoint(asio::ip::address::from_string("127.0.0.1"), real_agent_port))
        , lost_(lost)
    {
        agent_socket_.open(asio::ip::udp::v4());
        std::random_device rd;
        random_.seed(rd());
    }

    ~Gateway()
    {
    }


    void launch()
    {
        thread_ = std::thread(&Gateway::run, this);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    void stop()
    {
        io_service_.stop();
        thread_.join();
    }

private:
    static const size_t MESSAGE_LENGTH = 4096;
    void run()
    {
        listen_client_message();
        listen_agent_message();

        io_service_.run();
    }

    void listen_client_message()
    {
        using namespace std::placeholders;
        client_socket_.async_receive_from(asio::buffer(client_message_, MESSAGE_LENGTH), client_endpoint_,
        std::bind(&Gateway::handle_client_message, this, _1, _2));
    }

    void listen_agent_message()
    {
        using namespace std::placeholders;
        agent_socket_.async_receive_from(asio::buffer(agent_message_, MESSAGE_LENGTH), agent_endpoint_,
        std::bind(&Gateway::handle_agent_message, this, _1, _2));
    }

    void handle_client_message(const asio::error_code& error, size_t length)
    {
        if(!error)
        {
            if(!check_lost(length, &last_client_message_lost_))
            {
                agent_socket_.send_to(asio::buffer(client_message_, length), agent_endpoint_);
            }
        }
        listen_client_message();
    }

    void handle_agent_message(const asio::error_code& error, size_t length)
    {
        if(!error)
        {
            if(!check_lost(length, &last_agent_message_lost_))
            {
                client_socket_.send_to(asio::buffer(agent_message_, length), client_endpoint_);
            }
        }
        listen_agent_message();
    }

    bool check_lost(size_t length, bool* last_message_lost)
    {
        if(length == 7) //header.
        {
            *last_message_lost = msg_lost_(random_) < lost_;
        }
        else
        {
            if(*last_message_lost)
            {
                std::cout << "[Message with " << length << " bytes lost]" << std::endl;
            }
        }
        return *last_message_lost;
    }

    std::mt19937 random_;
    static std::uniform_real_distribution<float> msg_lost_;
    bool last_client_message_lost_;
    bool last_agent_message_lost_;

    asio::io_service io_service_;
    asio::ip::udp::socket client_socket_;
    asio::ip::udp::socket agent_socket_;

    char client_message_[MESSAGE_LENGTH];
    char agent_message_[MESSAGE_LENGTH];

    asio::ip::udp::endpoint client_endpoint_;
    asio::ip::udp::endpoint agent_endpoint_;

    float lost_;
    std::thread thread_;
};

std::uniform_real_distribution<float> Gateway::msg_lost_(0.0f, 1.0f);


class InteractionTest : public ::testing::Test
{
public:
    static const uint8_t IP[4];
    static const uint16_t AGENT_PORT = 2018;

    InteractionTest()
        : agent_(AGENT_PORT)
    {}

    ~InteractionTest()
    {
        agent_.stop();
    }

    AgentT& agent() { return agent_; }

private:
    AgentT agent_;
};

const uint8_t InteractionTest::IP[4] = {127, 0, 0, 1};

void create_xrce_object_test(AgentT& agent, Client& client)
{
    std::array<uint8_t, 4> client_key;
    std::copy(std::begin(client.get_client_key().data), std::end(client.get_client_key().data), std::begin(client_key));

    /* Launch Agent. */
    ASSERT_EQ(agent.client_disconnected(client_key), true);
    agent.launch();

    /* Init session. */
    ASSERT_EQ(client.init_session(), true);
    ASSERT_EQ(agent.client_connected(client_key), true);

    /* Create participant. */
    ASSERT_EQ(client.create_participant(), true);
    ObjectId participant_id = client.get_participant_id();
    ASSERT_EQ(agent.object_created(client_key, {participant_id.data[0], participant_id.data[1]}), true);

    /* Create topic. */
    ASSERT_EQ(client.create_topic(), true);
    ObjectId topic_id = client.get_topic_id();
    ASSERT_EQ(agent.object_created(client_key, {topic_id.data[0], topic_id.data[1]}), true);

    /* Create publisher. */
    ASSERT_EQ(client.create_publisher(), true);
    ObjectId publisher_id = client.get_publisher_id();
    ASSERT_EQ(agent.object_created(client_key, {publisher_id.data[0], publisher_id.data[1]}), true);

    /* Create datawriter. */
    ASSERT_EQ(client.create_datawriter(), true);
    ObjectId datawriter_id = client.get_datawriter_id();
    ASSERT_EQ(agent.object_created(client_key, {datawriter_id.data[0], datawriter_id.data[1]}), true);

    /* Create subscriber. */
    ASSERT_EQ(client.create_subscriber(), true);
    ObjectId subscriber_id = client.get_subscriber_id();
    ASSERT_EQ(agent.object_created(client_key, {subscriber_id.data[0], subscriber_id.data[1]}), true);

    /* Create datareader. */
    ASSERT_EQ(client.create_datareader(), true);
    ObjectId datareader_id = client.get_datareader_id();
    ASSERT_EQ(agent.object_created(client_key, {datareader_id.data[0], datareader_id.data[1]}), true);

    /* Delete datareader. */
    ASSERT_EQ(client.delete_object(datareader_id), true);
    ASSERT_EQ(agent.object_deleted(client_key, {datareader_id.data[0], datareader_id.data[1]}), true);

    /* Delete subscriber. */
    ASSERT_EQ(client.delete_object(subscriber_id), true);
    ASSERT_EQ(agent.object_deleted(client_key, {subscriber_id.data[0], subscriber_id.data[1]}), true);

    /* Delete datawriter. */
    ASSERT_EQ(client.delete_object(datawriter_id), true);
    ASSERT_EQ(agent.object_deleted(client_key, {datawriter_id.data[0], datawriter_id.data[1]}), true);

    /* Delete publisher. */
    ASSERT_EQ(client.delete_object(publisher_id), true);
    ASSERT_EQ(agent.object_deleted(client_key, {publisher_id.data[0], publisher_id.data[1]}), true);

    /* Delete topic. */
    ASSERT_EQ(client.delete_object(topic_id), true);
    ASSERT_EQ(agent.object_deleted(client_key, {topic_id.data[0], topic_id.data[1]}), true);

    /* Delete participant. */
    ASSERT_EQ(client.delete_object(participant_id), true);
    ASSERT_EQ(agent.object_deleted(client_key, {participant_id.data[0], participant_id.data[1]}), true);

    /* Close session. */
    ASSERT_EQ(client.close_session(), true);
    ASSERT_EQ(agent.client_disconnected(client_key), true);
}

void publisher_subscriber_test(AgentT& agent, Publisher& publisher, Subscriber& subscriber)
{
    agent.launch();
    ASSERT_EQ(publisher.init(), true);
    ASSERT_EQ(subscriber.init(), true);

    std::thread publisher_thread = std::thread(&Publisher::launch, &publisher);
    std::thread subscriber_thread = std::thread(&Subscriber::launch, &subscriber);

    subscriber_thread.join();
    publisher_thread.join();

    publisher.close();
    subscriber.close();

    HelloWorld topic_published = publisher.get_topic();
    HelloWorld topic_read = subscriber.get_topic();
    ASSERT_EQ(topic_published.index == topic_read.index, true);
    ASSERT_EQ(memcmp(topic_published.message, topic_read.message, strlen(topic_published.message)) == 0, true);

}

TEST_F(InteractionTest, CreateXRCEObjects)
{
    Client client({{0xAA, 0xBB, 0xCC, 0xDD}}, InteractionTest::IP, InteractionTest::AGENT_PORT);

    create_xrce_object_test(agent(), client);
}

TEST_F(InteractionTest, PublisherSubscriber)
{
    Publisher publisher({{0xAA, 0xAA, 0xAA, 0xAA}}, InteractionTest::IP, InteractionTest::AGENT_PORT);
    Subscriber subscriber({{0xBB, 0xBB, 0xBB, 0xBB}}, InteractionTest::IP, InteractionTest::AGENT_PORT);

    publisher_subscriber_test(agent(), publisher, subscriber);
}

TEST_F(InteractionTest, CreateXRCEObjectsWithLost)
{
    Gateway gateway(2019, InteractionTest::AGENT_PORT, 0.1f);
    gateway.launch();

    Client client({{0xAA, 0xBB, 0xCC, 0xDD}}, InteractionTest::IP, 2019);

    create_xrce_object_test(agent(), client);
    gateway.stop();
}

TEST_F(InteractionTest, PublisherSubscriberWithLost)
{
    Gateway gateway_pub(2019, InteractionTest::AGENT_PORT, 0.1f);
    Gateway gateway_sub(2020, InteractionTest::AGENT_PORT, 0.1f);
    gateway_pub.launch();
    gateway_sub.launch();

    Publisher publisher({{0xAA, 0xAA, 0xAA, 0xAA}}, InteractionTest::IP, 2019);
    Subscriber subscriber({{0xBB, 0xBB, 0xBB, 0xBB}}, InteractionTest::IP, 2020);

    publisher_subscriber_test(agent(), publisher, subscriber);
    gateway_pub.stop();
    gateway_sub.stop();
}

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
