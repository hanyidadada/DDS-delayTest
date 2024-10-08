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
                file << "Average Latency: " << avg_latency << " ns" << std::endl;
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

int main(int argc, char const *argv[])
{
    if(argc < 2) {
        printf("Error Use.\n");
        exit(1);
    }

    int size = atoi(argv[1]);
    DomainParticipantQos participant_qos;
    participant_qos.name("Participant_sub");
    DomainParticipant* participant = DomainParticipantFactory::get_instance()->create_participant(0, participant_qos);

    if (participant == nullptr)
    {
        std::cerr << "Error creating participant" << std::endl;
        return 1;
    }

    TypeSupport type(new TimeTestPubSubType());
    type.register_type(participant);

    TopicQos topic_qos;
    Topic* topic = participant->create_topic("TimeTestTopic", type.get_type_name(), topic_qos);

    if (topic == nullptr)
    {
        std::cerr << "Error creating topic" << std::endl;
        return 1;
    }

    SubscriberQos subscriber_qos;
    Subscriber* subscriber = participant->create_subscriber(subscriber_qos);

    if (subscriber == nullptr)
    {
        std::cerr << "Error creating subscriber" << std::endl;
        return 1;
    }

    DataReaderQos datareader_qos;
    TimeTestListener listener;
    DataReader* reader = subscriber->create_datareader(topic, datareader_qos, &listener);

    if (reader == nullptr)
    {
        std::cerr << "Error creating datareader" << std::endl;
        return 1;
    }

    std::this_thread::sleep_for(seconds(15));  // 等待足够时间接收所有数据
    std::string filename;
    switch (size)
    {
    case 1:
        filename = "latencies1B";
        break;
    case 2:
        filename = "latencies10B";
        break;
    case 3:
        filename = "latencies100B";
        break;
    case 4:
        filename = "latencies1KB";
        break;
    case 5:
        filename = "latencies10KB";
        break;
    case 6:
        filename = "latencies100KB";
        break;
    case 7:
        filename = "latencies1MB";
        break;
    case 8:
        filename = "latencies10MB";
        break;
    default:
        break;
    }

    listener.save_results(filename);

    participant->delete_contained_entities();
    DomainParticipantFactory::get_instance()->delete_participant(participant);

    return 0;
}
