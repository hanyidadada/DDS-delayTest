#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <random>
#include "TimeTestPubSubTypes.h"

using namespace eprosima::fastdds::dds;
using namespace std::chrono;

std::string generate_random_string(std::size_t length)
{
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string random_string;
    for (std::size_t i = 0; i < length; ++i) {
        random_string += characters[distribution(generator)];
    }

    return random_string;
}

int main()
{
    // 创建DomainParticipant
    DomainParticipantQos participant_qos;
    participant_qos.name("Participant_pub");
    DomainParticipant* participant = DomainParticipantFactory::get_instance()->create_participant(0, participant_qos);

    if (participant == nullptr)
    {
        std::cerr << "Error creating participant" << std::endl;
        return 1;
    }

    // 注册类型
    TypeSupport type(new TimeTestPubSubType());
    type.register_type(participant);

    // 创建Topic
    TopicQos topic_qos;
    topic_qos.history().kind = KEEP_LAST_HISTORY_QOS;
    topic_qos.history().depth = 1;
    topic_qos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
    // topic_qos.latency_budget().duration = eprosima::fastrtps::Duration_t(0, 10000000); // 10 ms
    Topic* topic = participant->create_topic("TimeTestTopic", type.get_type_name(), topic_qos);

    if (topic == nullptr)
    {
        std::cerr << "Error creating topic" << std::endl;
        return 1;
    }

    // 创建Publisher
    PublisherQos publisher_qos;
    Publisher* publisher = participant->create_publisher(publisher_qos);

    if (publisher == nullptr)
    {
        std::cerr << "Error creating publisher" << std::endl;
        return 1;
    }

    // 创建DataWriter
    DataWriterQos datawriter_qos;
    datawriter_qos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
    datawriter_qos.history().kind = KEEP_LAST_HISTORY_QOS;
    datawriter_qos.history().depth = 1;
    datawriter_qos.latency_budget().duration = eprosima::fastrtps::Duration_t(0, 10000000); // 10 ms
    datawriter_qos.destination_order().kind = BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS;
    DataWriter* writer = publisher->create_datawriter(topic, datawriter_qos);

    if (writer == nullptr)
    {
        std::cerr << "Error creating datawriter" << std::endl;
        return 1;
    }

    // 发送消息
    TimeTest data;
    data.message(generate_random_string(1));

    for (int i = 0; i < 1000; ++i)
    {
        data.timestamp(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count());
        writer->write(&data);
        std::this_thread::sleep_for(milliseconds(10));  // 等待10ms发送下一条消息
    }

    // 清理
    participant->delete_contained_entities();
    DomainParticipantFactory::get_instance()->delete_participant(participant);

    return 0;
}
