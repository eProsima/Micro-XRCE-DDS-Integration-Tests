#include <gtest/gtest.h>

#include <agent/Root.h>
#include <micrortps/client/xrce_client.h>

#include "HelloWorld.h"

#define MAX_NUM_ATTEMPTS    100
#define MAX_TIME_WAIT        10
#define TOPIC_PREFIX       0x01

namespace eprosima { namespace micrortps { namespace testing {

/****************************************************************************************
 * Agent
 ****************************************************************************************/
class AgentT
{
public:
    AgentT()
        : agent_(root()),
          port_(2019)
    {
    }

    void launch()
    {
        agent_.init(port_);
        thread_ = std::thread(&Agent::run, &agent_);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    void stop()
    {
        agent_.stop();
        thread_.join();
        agent_.abort_execution();
    }

    bool client_disconnected(const dds::xrce::ClientKey& client_key)
    {
        return  agent_.get_client(client_key) == nullptr;
    }

    bool client_connected(const dds::xrce::ClientKey& client_key)
    {
        ProxyClient* client = nullptr;
        for (uint16_t i = 0; i < MAX_NUM_ATTEMPTS && client == nullptr; ++i)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(MAX_TIME_WAIT));
            client = agent_.get_client(client_key);
        }
        return client != nullptr;
    }

    bool object_created(const dds::xrce::ClientKey& client_key, const dds::xrce::ObjectId& object_id)
    {
        XRCEObject* object = nullptr;
        ProxyClient* client = agent_.get_client(client_key);
        if (nullptr != client)
        {
            for (uint16_t i = 0; i < MAX_NUM_ATTEMPTS; ++i)
            {
                std::this_thread::sleep_for(std::chrono::microseconds(MAX_TIME_WAIT));
                object = client->get_object(object_id);
            }
        }
        return object != nullptr;
    }

    bool object_deleted(const dds::xrce::ClientKey& client_key, const dds::xrce::ObjectId& object_id)
    {
        XRCEObject* object = nullptr;
        ProxyClient* client = agent_.get_client(client_key);
        if (nullptr != client)
        {
            for (uint16_t i = 0; i < MAX_NUM_ATTEMPTS; ++i)
            {
                std::this_thread::sleep_for(std::chrono::microseconds(MAX_TIME_WAIT));
                object = client->get_object(object_id);
            }
        }
        return (object == nullptr) && (client != nullptr);
    }

private:
    std::thread thread_;
    eprosima::micrortps::Agent& agent_;
    uint16_t port_;
};

/****************************************************************************************
 * Client
 ****************************************************************************************/
class Client
{
public:
    Client(const ClientKey& client_key, const uint8_t ip[4], const uint16_t port)
        : session_(),
          client_key_(client_key),
          ip_{ip[0], ip[1], ip[2], ip[4]},
          port_(port)
    {
    }

    bool init_session()
    {
        return new_udp_session(&session_, 0x01, client_key_, ip_, port_, NULL, NULL)
                && init_session_sync(&session_);
    }

    bool close_session()
    {
        return close_session_sync(&session_);
    }

    bool create_participant()
    {
        return create_participant_sync_by_ref(&session_, participant_id_, "default_participant", false, false);
    }

    bool create_topic()
    {
        return create_topic_sync_by_xml(&session_, topic_id_, topic_xml_, participant_id_, false, false);
    }

    bool create_publisher()
    {
        return create_publisher_sync_by_xml(&session_, publisher_id_, publisher_xml_, participant_id_, false, false);
    }

    bool create_subscriber()
    {
        return create_subscriber_sync_by_xml(&session_, subscriber_id_, subscriber_xml_, participant_id_, false, false);
    }

    bool create_datawriter()
    {
        return create_datawriter_sync_by_xml(&session_, datawriter_id_, datawriter_xml_, publisher_id_, false, false);
    }

    bool create_datareader()
    {
        return create_datareader_sync_by_xml(&session_, datareader_id_, datareader_xml_, subscriber_id_, false, false);
    }

    bool delete_object(ObjectId id)
    {
        return delete_object_sync(&session_, id);
    }

    const ClientKey& get_client_key() const { return client_key_; }

    const ObjectId& get_participant_id()   const { return participant_id_; }
    const ObjectId& get_topic_id()         const { return topic_id_; }
    const ObjectId& get_publisher_id()     const { return publisher_id_; }
    const ObjectId& get_datawriter_id()    const { return datawriter_id_; }
    const ObjectId& get_subscriber_id()    const { return subscriber_id_; }
    const ObjectId& get_datareader_id()    const { return datareader_id_; }

protected:
    /* Session config. */
    Session session_;
    const ClientKey client_key_;
    const uint8_t ip_[4];
    const uint16_t port_;

    /* Object IDs. */
    static const ObjectId participant_id_;
    static const ObjectId topic_id_;
    static const ObjectId publisher_id_;
    static const ObjectId datawriter_id_;
    static const ObjectId subscriber_id_;
    static const ObjectId datareader_id_;

    /* XMLs. */
    static const char* topic_xml_;
    static const char* publisher_xml_;
    static const char* datawriter_xml_;
    static const char* subscriber_xml_;
    static const char* datareader_xml_;

    /* Utils. */
    static std::atomic<bool> running_cond_;
};

const ObjectId Client::participant_id_ = {{0x00, 0x01}};
const ObjectId Client::topic_id_       = {{0x00, 0x02}};
const ObjectId Client::publisher_id_   = {{0x00, 0x03}};
const ObjectId Client::datawriter_id_  = {{0x00, 0x05}};
const ObjectId Client::subscriber_id_  = {{TOPIC_PREFIX, 0x04}};
const ObjectId Client::datareader_id_  = {{TOPIC_PREFIX, 0x06}};

const char* Client::topic_xml_      = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
const char* Client::publisher_xml_  = {"<publisher name=\"MyPublisher\""};
const char* Client::datawriter_xml_ = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
const char* Client::subscriber_xml_ = {"<publisher name=\"MySubscriber\""};
const char* Client::datareader_xml_ = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};

std::atomic<bool> Client::running_cond_{true};

/****************************************************************************************
 * Publisher
 ****************************************************************************************/
class Publisher : public Client
{
public:
    Publisher(const ClientKey& client_key, const uint8_t ip[4], const uint16_t port)
        : Client(client_key, ip, port) {}

    bool init()
    {
        bool result = true;
        result &= new_udp_session(&session_, 0x01, client_key_, ip_, port_, NULL, NULL);
        result &= init_session_sync(&session_);
        result &= create_participant_sync_by_ref(&session_, participant_id_, "default_participant", false, false);
        result &= create_topic_sync_by_xml(&session_, topic_id_, topic_xml_, participant_id_, false, false);
        result &= create_publisher_sync_by_xml(&session_, publisher_id_, publisher_xml_, participant_id_, false, false);
        result &= create_datawriter_sync_by_xml(&session_, datawriter_id_, datawriter_xml_, publisher_id_, false, false);
        return result;
    }

    void launch()
    {
        for (uint32_t i = 0; i < MAX_NUM_ATTEMPTS && running_cond_; ++i)
        {
            write_HelloWorld(&session_, datawriter_id_, STREAMID_BUILTIN_RELIABLE, &topic_);
            run_communication(&session_);
            std::this_thread::sleep_for(std::chrono::milliseconds(MAX_TIME_WAIT));
        }
    }

    const HelloWorld& get_topic() const { return topic_; }

private:
    char topic_msg_[32] = "Hello DDS World";
    const HelloWorld topic_ = {11, topic_msg_};
};

/****************************************************************************************
 * Subscriber
 ****************************************************************************************/
class Subscriber : public Client
{
public:
    Subscriber(const ClientKey& client_key, const uint8_t ip[4], uint16_t port)
        : Client(client_key, ip, port) {}

    bool init()
    {
        bool result = true;
        result &= new_udp_session(&session_, 0x01, client_key_, ip_, port_, on_topic, NULL);
        result &= init_session_sync(&session_);
        result &= create_participant_sync_by_ref(&session_, participant_id_, "default_participant", false, false);
        result &= create_topic_sync_by_xml(&session_, topic_id_, topic_xml_, participant_id_, false, false);
        result &= create_subscriber_sync_by_xml(&session_, subscriber_id_, subscriber_xml_, participant_id_, false, false);
        result &= create_datareader_sync_by_xml(&session_, datareader_id_, datareader_xml_, subscriber_id_, false, false);
        return result;
    }

    void launch()
    {
        for (uint32_t i = 0; i < MAX_NUM_ATTEMPTS && running_cond_; ++i)
        {
            read_data_sync(&session_, datareader_id_, STREAMID_BUILTIN_RELIABLE);
            run_communication(&session_);
            std::this_thread::sleep_for(std::chrono::milliseconds(MAX_TIME_WAIT));
        }
        running_cond_ = false;
    }

    const HelloWorld& get_topic() const { return topic_; }

private:
    static void on_topic(ObjectId id, MicroBuffer* serialized_topic, void* /*args*/)
    {
        HelloWorld topic;
        if (TOPIC_PREFIX == id.data[0])
        {
            deserialize_HelloWorld_topic(serialized_topic, &topic);
            running_cond_ = false;
        }
        topic_.index = topic.index;
        memcpy(topic_.message, topic.message, strlen(topic.message));
        return;
    }

private:
    static char topic_msg_[32];
    static HelloWorld topic_;
};

char Subscriber::topic_msg_[32] = {'\0'};
HelloWorld Subscriber::topic_ = {0, Subscriber::topic_msg_};


class InteractionTest : public ::testing::Test
{
public:
    InteractionTest()
        : agent_(),
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

TEST_F(InteractionTest, ClientToAgent)
{
    const ClientKey& key = client().get_client_key();
    const dds::xrce::ClientKey client_key = {key.data[0], key.data[1], key.data[2], key.data[3]};

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

} } } // namespace eprosima::micrortps::testing

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
