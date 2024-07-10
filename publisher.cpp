#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/core/policy/QosPolicies.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <chrono>
#include <iostream>
#include <thread>

#include "HelloWorldPubSubTypes.h"

using namespace eprosima::fastdds::dds;

class PubListener : public DataWriterListener
{
public:
    void on_publication_matched(DataWriter*, const PublicationMatchedStatus& info) override
    {
        if (info.current_count_change == 1)
        {
            std::cout << "Publisher matched." << std::endl;
            matched_ = true;
        }
        else if (info.current_count_change == -1)
        {
            std::cout << "Publisher unmatched." << std::endl;
            matched_ = false;
        }
    }

    bool matched_ = false;
};

int main()
{
    DomainParticipantQos participantQos;
    participantQos.transport().use_builtin_transports = true;
    DomainParticipant* participant = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);
    TypeSupport type(new HelloWorldPubSubType());
    type.register_type(participant);

    Topic* topic = participant->create_topic("HelloWorldTopic", type.get_type_name(), TOPIC_QOS_DEFAULT);
    Publisher* publisher = participant->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
    PubListener listener;
    DataWriterQos writerQos;
    writerQos.history().kind = KEEP_LAST_HISTORY_QOS;
    writerQos.history().depth = 1;
    writerQos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    writerQos.durability().kind = VOLATILE_DURABILITY_QOS;
    DataWriter* writer = publisher->create_datawriter(topic, writerQos, &listener);

    HelloWorld hello;
    hello.index(1);
    hello.message("Hello, World!");

    const int num_messages = 1000;  // 发送消息的数量
    std::vector<std::chrono::nanoseconds> send_times(num_messages);

    while (!listener.matched_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    for (int i = 0; i < num_messages; ++i)
    {
        auto start = std::chrono::high_resolution_clock::now();
        writer->write(&hello);
        auto end = std::chrono::high_resolution_clock::now();
        send_times[i] = end - start;

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 控制发送频率
    }

    participant->delete_contained_entities();
    DomainParticipantFactory::get_instance()->delete_participant(participant);

    return 0;
}
