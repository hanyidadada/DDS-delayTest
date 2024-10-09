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

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>

using namespace eprosima::fastdds::dds;

std::string generate_random_string(size_t length)
{
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, characters.size() - 1);
    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i)
    {
        result += characters[dist(rng)];
    }
    return result;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: TimeTestPub <message_length>" << std::endl;
        return 1;
    }

    size_t message_length = std::stoul(argv[1]);

    // 初始化DomainParticipant
    DomainParticipantQos participant_qos;
    participant_qos.name("TimeTestPubParticipant");
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
    // writer_qos.history().kind = KEEP_LAST_HISTORY_QOS;
    // writer_qos.history().depth = 1;
    // writer_qos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
    writer_qos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    // writer_qos.durability().kind = VOLATILE_DURABILITY_QOS;

    DataReaderQos reader_qos;
    // reader_qos.history().kind = KEEP_LAST_HISTORY_QOS;
    // reader_qos.history().depth = 1;
    // reader_qos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
    reader_qos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    // reader_qos.durability().kind = VOLATILE_DURABILITY_QOS;

    // 创建DataWriter和DataReader
    DataWriter* writer = publisher->create_datawriter(pub_topic, writer_qos);
    DataReader* reader = subscriber->create_datareader(sub_topic, reader_qos);

    // 等待订阅者匹配
    std::cout << "Waiting for subscriber..." << std::endl;

    PublicationMatchedStatus pub_status;
    // SubscriptionMatchedStatus sub_status;
    while (writer->get_publication_matched_status(pub_status) == ReturnCode_t::RETCODE_OK &&
           pub_status.current_count == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // 打开结果文件，并注明单位
    std::ofstream result_file("result_" + std::to_string(message_length) + ".txt");
    result_file << "Round-trip time (nanoseconds)" << std::endl;

    TimeTestPub pub_data;
    TimeTestSub sub_data;
    SampleInfo info;
    pub_data.message(generate_random_string(message_length));
    std::cout << "start test" << std::endl;
    for (int i = 0; i < 1000; ++i)
    {
        // 生成随机字符串

        // 获取当前时间戳（纳秒）
        auto send_time = std::chrono::steady_clock::now();
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(send_time.time_since_epoch()).count();
        pub_data.timestamp(timestamp);

        // 发送消息
        writer->write(&pub_data);

        // 等待回复
        bool received = false;
        while (!received)
        {
            if (reader->read_next_sample(&sub_data, &info) == ReturnCode_t::RETCODE_OK)
            {
                if (info.valid_data && sub_data.timestamp() == pub_data.timestamp())
                {
                    auto receive_time = std::chrono::steady_clock::now();
                    auto rtt = std::chrono::duration_cast<std::chrono::nanoseconds>(receive_time - send_time).count();
                    result_file << rtt/2000 << std::endl;
                    received = true;
                }
            }
        }

        // 间隔10ms
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // 清理资源
    result_file.close();
    participant->delete_contained_entities();
    DomainParticipantFactory::get_instance()->delete_participant(participant);

    return 0;
}
