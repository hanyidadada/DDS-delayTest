#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

#include "TimeTestPubPubSubTypes.h"
#include "TimeTestSubPubSubTypes.h"

#include <iostream>
#include <thread>

using namespace eprosima::fastdds::dds;

int main()
{
    // 初始化DomainParticipant
    DomainParticipantQos participant_qos;
    participant_qos.name("TimeTestSubParticipant");
    auto participant = DomainParticipantFactory::get_instance()->create_participant(0, participant_qos);

    // 注册类型
    TypeSupport pub_type(new TimeTestPubPubSubType());
    pub_type.register_type(participant);

    TypeSupport sub_type(new TimeTestSubPubSubType());
    sub_type.register_type(participant);

    // 创建Publisher和Subscriber
    Publisher* publisher = participant->create_publisher(PUBLISHER_QOS_DEFAULT);
    Subscriber* subscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT);

    // 创建Topic
    Topic* pub_topic = participant->create_topic("TimeTestPubTopic", "TimeTestPub", TOPIC_QOS_DEFAULT);
    Topic* sub_topic = participant->create_topic("TimeTestSubTopic", "TimeTestSub", TOPIC_QOS_DEFAULT);

    // 设置QoS以降低时延
    DataWriterQos writer_qos;
    writer_qos.history().kind = KEEP_LAST_HISTORY_QOS;
    writer_qos.history().depth = 1;
    writer_qos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
    writer_qos.durability().kind = VOLATILE_DURABILITY_QOS;

    DataReaderQos reader_qos = subscriber->get_default_datareader_qos();
    reader_qos.history().kind = KEEP_LAST_HISTORY_QOS;
    reader_qos.history().depth = 1;
    reader_qos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
    reader_qos.durability().kind = VOLATILE_DURABILITY_QOS;

    // 创建DataWriter和DataReader
    DataWriter* writer = publisher->create_datawriter(sub_topic, writer_qos);
    DataReader* reader = subscriber->create_datareader(pub_topic, reader_qos);

    // 等待发布者匹配
    std::cout << "Waiting for publisher..." << std::endl;
    SubscriptionMatchedStatus sub_status;
    while (reader->get_subscription_matched_status(sub_status) == ReturnCode_t::RETCODE_OK &&
           sub_status.current_count == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    TimeTestPub pub_data;
    TimeTestSub sub_data;
    SampleInfo info;
    std::cout << "start test\n" << std::endl;
    while (true)
    {
        if (reader->read_next_sample(&pub_data, &info) == ReturnCode_t::RETCODE_OK)
        {
            if (info.valid_data)
            {
                // 填充并回传消息
                sub_data.timestamp(pub_data.timestamp());
                sub_data.message(pub_data.message());
                writer->write(&sub_data);
            }
        }
    }

    // 清理资源
    participant->delete_contained_entities();
    DomainParticipantFactory::get_instance()->delete_participant(participant);

    return 0;
}
