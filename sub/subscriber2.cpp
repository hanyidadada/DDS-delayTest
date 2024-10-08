#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include "TimeTestPubSubTypes.h"

using namespace eprosima::fastdds::dds;
using namespace std::chrono;

class TimeTestListener : public DataReaderListener
{
public:
    TimeTestListener() : count(0), total_latency(0) {}

    void on_data_available(DataReader* reader) override
    {
        SampleInfo info;
        TimeTest data;
        if (reader->take_next_sample(&data, &info) == ReturnCode_t::RETCODE_OK)
        {
            if (info.instance_state == ALIVE_INSTANCE_STATE)
            {
                auto now = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
                auto latency = now - data.timestamp();
                latencies.push_back(latency);
                total_latency += latency;
                ++count;
            }
        }
    }

    void save_results(const std::string& filename)
    {
        std::ofstream file(filename);
        if (file.is_open())
        {
            for (const auto& latency : latencies)
            {
                file << latency << std::endl;
            }
            if (count > 0)
            {
                auto avg_latency = total_latency / count;
                auto min_latency = *std::min_element(latencies.begin(), latencies.end());
                auto max_latency = *std::max_element(latencies.begin(), latencies.end());

                std::vector<long long> jitter;
                for (std::size_t i = 1; i < latencies.size(); ++i)
                {
                    jitter.push_back(std::abs(latencies[i] - latencies[i - 1]));
                }
                auto max_jitter = jitter.empty() ? 0 : *std::max_element(jitter.begin(), jitter.end());

                file << "Average Latency: " << avg_latency << " ns" << std::endl;
                file << "Minimum Latency: " << min_latency << " ns" << std::endl;
                file << "Maximum Latency: " << max_latency << " ns" << std::endl;
                file << "Maximum Jitter: " << max_jitter << " ns" << std::endl;
            }
            else
            {
                file << "No data received." << std::endl;
            }
            file.close();
        }
        else
        {
            std::cerr << "Unable to open file" << std::endl;
        }
    }

private:
    int count;
    long long total_latency;
    std::vector<long long> latencies;
};

int main()
{
    // 创建DomainParticipant
    DomainParticipantQos participant_qos;
    participant_qos.name("Participant_sub");
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
    // topic_qos.latency_budget().duration = Duration_t(0, 10000000); // 10 ms
    Topic* topic = participant->create_topic("TimeTestTopic", type.get_type_name(), topic_qos);

    if (topic == nullptr)
    {
        std::cerr << "Error creating topic" << std::endl;
        return 1;
    }

    // 创建Subscriber
    SubscriberQos subscriber_qos;
    Subscriber* subscriber = participant->create_subscriber(subscriber_qos);

    if (subscriber == nullptr)
    {
        std::cerr << "Error creating subscriber" << std::endl;
        return 1;
    }

    // 创建DataReader
    DataReaderQos datareader_qos;
    datareader_qos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
    datareader_qos.history().kind = KEEP_LAST_HISTORY_QOS;
    datareader_qos.history().depth = 1;
    // datareader_qos.latency_budget().duration = Duration_t(0, 10000000); // 10 ms

    TimeTestListener listener;
    DataReader* reader = subscriber->create_datareader(topic, datareader_qos, &listener);

    if (reader == nullptr)
    {
        std::cerr << "Error creating datareader" << std::endl;
        return 1;
    }

    std::this_thread::sleep_for(seconds(15));  // 等待足够时间接收所有数据

    listener.save_results("latencies.txt");

    // 清理
    participant->delete_contained_entities();
    DomainParticipantFactory::get_instance()->delete_participant(participant);

    return 0;
}
