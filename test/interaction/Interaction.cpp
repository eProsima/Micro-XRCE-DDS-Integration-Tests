#include <gtest/gtest.h>
#include <ClientInteraction.hpp>
#include <AgentInteraction.hpp>

class InteractionTest : public ::testing::Test
{
public:
    InteractionTest()
        : agent_(port_),
          client_(client_key_, ip_, port_),
          publisher_(publisher_key_, ip_, port_),
          subscriber_(subscriber_key_, ip_, port_)
    {}

    ~InteractionTest()
    {
        agent().stop();
    }

    AgentT& agent() { return agent_; }
    Client& client() { return client_; }
    Publisher& publisher() { return publisher_; }
    Subscriber& subscriber() { return subscriber_; }

private:
    const ClientKey client_key_{{0xAA, 0xBB, 0xCC, 0xDD}};
    const ClientKey publisher_key_{{0xAA, 0xAA, 0xAA, 0xAA}};
    const ClientKey subscriber_key_{{0xBB, 0xBB, 0xBB, 0xBB}};
    const uint8_t ip_[4] = {127, 0, 0, 1};
    const uint16_t port_ = 2019;

    AgentT agent_;
    Client client_;
    Publisher publisher_;
    Subscriber subscriber_;
};

TEST_F(InteractionTest, ClientToAgentCreates)
{
    const ClientKey& key = client().get_client_key();
    std::array<uint8_t, 4> client_key = {key.data[0], key.data[1], key.data[2], key.data[3]};

    /* Launch Agent. */
    ASSERT_EQ(agent().client_disconnected(client_key), true);
    agent().launch();

    /* Init session. */
    ASSERT_EQ(client().init_session(), true);
    ASSERT_EQ(agent().client_connected(client_key), true);

    /* Create participant. */
    ASSERT_EQ(client().create_participant(), true);
    ObjectId participant_id = client().get_participant_id();
    ASSERT_EQ(agent().object_created(client_key, {participant_id.data[0], participant_id.data[1]}), true);

    /* Create topic. */
    ASSERT_EQ(client().create_topic(), true);
    ObjectId topic_id = client().get_topic_id();
    ASSERT_EQ(agent().object_created(client_key, {topic_id.data[0], topic_id.data[1]}), true);

    /* Create publisher. */
    ASSERT_EQ(client().create_publisher(), true);
    ObjectId publisher_id = client().get_publisher_id();
    ASSERT_EQ(agent().object_created(client_key, {publisher_id.data[0], publisher_id.data[1]}), true);

    /* Create datawriter. */
    ASSERT_EQ(client().create_datawriter(), true);
    ObjectId datawriter_id = client().get_datawriter_id();
    ASSERT_EQ(agent().object_created(client_key, {datawriter_id.data[0], datawriter_id.data[1]}), true);

    /* Create subscriber. */
    ASSERT_EQ(client().create_subscriber(), true);
    ObjectId subscriber_id = client().get_subscriber_id();
    ASSERT_EQ(agent().object_created(client_key, {subscriber_id.data[0], subscriber_id.data[1]}), true);

    /* Create datareader. */
    ASSERT_EQ(client().create_datareader(), true);
    ObjectId datareader_id = client().get_datareader_id();
    ASSERT_EQ(agent().object_created(client_key, {datareader_id.data[0], datareader_id.data[1]}), true);

    /* Delete datareader. */
    ASSERT_EQ(client().delete_object(datareader_id), true);
    ASSERT_EQ(agent().object_deleted(client_key, {datareader_id.data[0], datareader_id.data[1]}), true);

    /* Delete subscriber. */
    ASSERT_EQ(client().delete_object(subscriber_id), true);
    ASSERT_EQ(agent().object_deleted(client_key, {subscriber_id.data[0], subscriber_id.data[1]}), true);

    /* Delete datawriter. */
    ASSERT_EQ(client().delete_object(datawriter_id), true);
    ASSERT_EQ(agent().object_deleted(client_key, {datawriter_id.data[0], datawriter_id.data[1]}), true);

    /* Delete publisher. */
    ASSERT_EQ(client().delete_object(publisher_id), true);
    ASSERT_EQ(agent().object_deleted(client_key, {publisher_id.data[0], publisher_id.data[1]}), true);

    /* Delete topic. */
    ASSERT_EQ(client().delete_object(topic_id), true);
    ASSERT_EQ(agent().object_deleted(client_key, {topic_id.data[0], topic_id.data[1]}), true);

    /* Delete participant. */
    ASSERT_EQ(client().delete_object(participant_id), true);
    ASSERT_EQ(agent().object_deleted(client_key, {participant_id.data[0], participant_id.data[1]}), true);

    /* Close session. */
    ASSERT_EQ(client().close_session(), true);
    ASSERT_EQ(agent().client_disconnected(client_key), true);
}

TEST_F(InteractionTest, ClientToClient)
{
    agent().launch();
    ASSERT_EQ(publisher().init(), true);
    ASSERT_EQ(subscriber().init(), true);

    std::thread publisher_thread = std::thread(&Publisher::launch, &publisher());
    std::thread subscriber_thread = std::thread(&Subscriber::launch, &subscriber());

    subscriber_thread.join();
    publisher_thread.join();

    HelloWorld topic_published = publisher().get_topic();
    HelloWorld topic_read = subscriber().get_topic();
    ASSERT_EQ(topic_published.index == topic_read.index, true);
    ASSERT_EQ(memcmp(topic_published.message, topic_read.message, strlen(topic_published.message)) == 0, true);
}

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
