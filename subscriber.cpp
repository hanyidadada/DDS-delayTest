#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/core/policy/QosPolicies.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <numeric>

#include "HelloWorldPubSubTypes.h"

using namespace eprosima::fastdds::dds;

class SubListener : public DataReaderListener
{
public:
    void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) override
    {
        if (info.current_count_change == 1)
        {
            std::cout << "Subscriber matched." << std::endl;
            matched_ = true;
        }
        else if (info.current_count_change == -1)
        {
            std::cout << "Subscriber unmatched." << std::endl;
            matched_ = false;
        }
    }

    void on_data_available(DataReader* reader) override
    {
        HelloWorld hello;
        SampleInfo info;

        if (reader->take_next_sample(&hello, &info) == ReturnCode_t::RETCODE_OK)
        {
            if (info.valid_data)
            {
                auto end = std::chrono::high_resolution_clock::now();
                auto delay = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_);
                delays_.push_back(delay);

                if (++received_messages_ >= num_messages_)
                {
                    // 计算统计数据
                    auto min_delay = *std::min_element(delays_.begin(), delays_.end());
                    auto max_delay = *std::max_element(delays_.begin(), delays_.end());
                    auto avg_delay = std::accumulate(delays_.begin(), delays_.end(), std::chrono::nanoseconds(0)) / num_messages_;
                    auto jitter = 0LL;
                    for (const auto& delay : delays_)
                    {
                        jitter += std::abs(delay.count() - avg_delay.count());
                    }
                    jitter /= num_messages_;

                    std::cout << "Min delay: " << min_delay.count() << " nanoseconds" << std::endl;
                    std::cout << "Max delay: " << max_delay.count() << " nanoseconds" << std::endl;
                    std::cout << "Avg delay: " << avg_delay.count() << " nanoseconds" << std::endl;
                    std::cout << "Jitter: " << jitter << " nanoseconds" << std::endl;

                    // 停止接收消息
                    receiving_ = false;
                }
            }
        }
    }

    bool matched_ = false;
    bool receiving_ = true;
    std::chrono::high_resolution_clock::time_point start_;
    int received_messages_ = 0;
    const int num_messages_ = 1000;
    std::vector<std::chrono::nanoseconds> delays_;
};

int main()
{
    DomainParticipantQos participantQos;
    participantQos.transport().use_builtin_transports = true;
    DomainParticipant* participant = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);
    TypeSupport type(new HelloWorldPubSubType());
    type.register_type(participant);

    Topic* topic = participant->create_topic("HelloWorldTopic", type.get_type_name(), TOPIC_QOS_DEFAULT);
    Subscriber* subscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
    SubListener listener;
    DataReaderQos readerQos;
    readerQos.history().kind = KEEP_LAST_HISTORY_QOS;
    readerQos.history().depth = 1;
    readerQos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    readerQos.durability().kind = VOLATILE_DURABILITY_QOS;
    DataReader* reader = subscriber->create_datareader(topic, readerQos, &listener);

    while (!listener.matched_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    listener.start_ = std::chrono::high_resolution_clock::now();

    std::cout << "Waiting for messages..." << std::endl;
    while (listener.receiving_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 等待所有消息到达
    }

    participant->delete_contained_entities();
    DomainParticipantFactory::get_instance()->delete_participant(participant);

    return 0;
}
