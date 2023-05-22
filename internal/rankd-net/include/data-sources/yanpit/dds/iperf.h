#ifndef YANP_IT_DDS_IPERF_H
#define YANP_IT_DDS_IPERF_H

#include <atomic>
#include <thread>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include "ulid.h"

#include "activenp.h"
#include "descriptors/iperfPubSubTypes.h"

using namespace eprosima::fastdds::dds;

class IperfPubListener : public DataWriterListener {
public:
    IperfPubListener() : _matched(0) {
    }
    ~IperfPubListener() override {
    }

    void on_publication_matched(DataWriter*, const PublicationMatchedStatus& info) override {
    }

    std::atomic_int _matched;
};

class IperfPublisher {
public:
    IperfPublisher() : _participant(nullptr), _publisher(nullptr), _topic(nullptr), _writer(nullptr) {
    }

    virtual ~IperfPublisher() {
        if (_writer != nullptr) {
            _publisher->delete_datawriter(_writer);
        }
        if (_publisher != nullptr) {
            _participant->delete_publisher(_publisher);
        }
        if (_topic != nullptr) {
            _participant->delete_topic(_topic);
        }

        DomainParticipantFactory::get_instance()->delete_participant(_participant);
    }

    void generate_message(const std::map<ulid::ULID, iperf::Server*>* server_entities, const std::map<ulid::ULID, iperf::Client*>* client_entities) {
        // Set timestamp.
        unsigned long timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        _message_structure.timestamp(timestamp);

        // Get servers information.
        std::vector<Iperf::IperfEntity> servers;
        for (const auto& [server_id, server] : *server_entities) {
            Iperf::IperfEntity server_entity;
            const iperf::Result* result;
            try {
                result = iperf::get_results(server);
            } catch (const iperf::Exception& e) {
                continue;
            }

            // Define header.
            Iperf::Header header;
            header.cookie(result->header().cookie);
            Iperf::Connection connection_item;
            std::vector<Iperf::ConnectionStatus> connections;
            for (const auto& connection : result->header().connection.connections) {
                Iperf::ConnectionStatus connection_status;
                connection_status.socket_number(connection.socket_number);
                connection_status.local_host(connection.local_host);
                connection_status.local_port(connection.local_port);
                connection_status.remote_host(connection.remote_host);
                connection_status.remote_port(connection.remote_port);
                connections.push_back(connection_status);
            }
            connection_item.connections(connections);
            Iperf::ConnectionIntention intention;
            intention.host(result->header().connection.intention.host);
            intention.port(result->header().connection.intention.port);
            connection_item.intention(intention);
            header.connection_item(connection_item);
            Iperf::BootstrapInformation information;
            information.version(result->header().information.version);
            information.system_information(result->header().information.system_information);
            information.tcp_mss_default(result->header().information.tcp_mss_default);
            information.target_bitrate(result->header().information.target_bitrate);
            information.sock_bufsize(result->header().information.sock_bufsize);
            information.sndbuf_actual(result->header().information.sndbuf_actual);
            information.rcvbuf_actual(result->header().information.rcvbuf_actual);
            information.protocol(result->header().information.protocol == iperf::TCP ? Iperf::ProtocolType::TCP : Iperf::ProtocolType::UDP);
            information.number_of_streams(result->header().information.number_of_streams);
            information.block_size(result->header().information.block_size);
            information.omitted_seconds(result->header().information.omitted_seconds);
            information.duration(result->header().information.duration);
            information.bytes(result->header().information.bytes);
            information.blocks(result->header().information.blocks);
            information.reverse_mode(result->header().information.reverse_mode);
            information.tos(result->header().information.tos);
            header.information(information);
            header.starting_time(result->header().starting_time.time_since_epoch().count());
            server_entity.header_item(header);

            // Set intervals.
            std::vector<Iperf::Interval> intervals;
            for (const auto& interval_item : result->intervals()) {
                Iperf::Interval interval;
                std::vector<Iperf::Stream> streams;
                for (const auto& stream : interval_item.streams) {
                    Iperf::Stream stream_item;
                    stream_item.socket_number(stream.socket_number);
                    stream_item.start(stream.start);
                    stream_item.end(stream.end);
                    stream_item.range(stream.range);
                    stream_item.bytes(stream.bytes);
                    stream_item.bits_per_second(stream.bits_per_second);
                    stream_item.omitted(stream.omitted);
                    stream_item.sender(stream.sender);
                    streams.push_back(stream_item);
                }
                interval.streams(streams);
                intervals.push_back(interval);
            }
            server_entity.intervals(intervals);

            // Set cumulative intervals.
            std::vector<Iperf::CumulativeInterval> cumulative_intervals;
            for (const auto& interval_item : result->cumulative_intervals()) {
                Iperf::CumulativeInterval cumulative_interval;
                cumulative_interval.start(interval_item.start);
                cumulative_interval.end(interval_item.end);
                cumulative_interval.range(interval_item.range);
                cumulative_interval.bytes(interval_item.bytes);
                cumulative_interval.bits_per_second(interval_item.bits_per_second);
                cumulative_intervals.push_back(cumulative_interval);
            }
            server_entity.cumulative_intervals(cumulative_intervals);

            // Add report data.
            Iperf::Report end_report;
            std::vector<Iperf::ReportedStreamsPair> streams;
            for (const auto& [sending_stream_item, receiving_stream_item] : result->end_report().streams) {
                Iperf::ReportedStreamsPair reported_pair;
                Iperf::ReportedSendingStream sending_stream;
                sending_stream.socket_number(sending_stream_item.socket_number);
                sending_stream.start(sending_stream_item.start);
                sending_stream.end(sending_stream_item.end);
                sending_stream.range(sending_stream_item.range);
                sending_stream.bytes(sending_stream_item.bytes);
                sending_stream.bits_per_second(sending_stream_item.bits_per_second);
                sending_stream.sender(sending_stream_item.sender);
                reported_pair.sending_stream(sending_stream);
                Iperf::ReportedReceivingStream receiving_stream;
                receiving_stream.socket_number(receiving_stream_item.socket_number);
                receiving_stream.start(receiving_stream_item.start);
                receiving_stream.end(receiving_stream_item.end);
                receiving_stream.range(receiving_stream_item.range);
                receiving_stream.bytes(receiving_stream_item.bytes);
                receiving_stream.bits_per_second(receiving_stream_item.bits_per_second);
                receiving_stream.sender(receiving_stream_item.sender);
                reported_pair.receiving_stream(receiving_stream);
                streams.push_back(reported_pair);
            }
            end_report.streams(streams);
            Iperf::ReportedSummariesPair summaries;
            auto [summary_sending_stream, summary_receiving_stream] = result->end_report().stream_summary;
            Iperf::ReportedSendingSummary sending_summary;
            sending_summary.start(summary_sending_stream.start);
            sending_summary.end(summary_sending_stream.end);
            sending_summary.range(summary_sending_stream.range);
            sending_summary.bytes(summary_sending_stream.bytes);
            sending_summary.bits_per_second(summary_sending_stream.bits_per_second);
            sending_summary.sender(summary_sending_stream.sender);
            summaries.sending_summary(sending_summary);
            Iperf::ReportedReceivingSummary receiving_summary;
            receiving_summary.start(summary_receiving_stream.start);
            receiving_summary.end(summary_receiving_stream.end);
            receiving_summary.range(summary_receiving_stream.range);
            receiving_summary.bytes(summary_receiving_stream.bytes);
            receiving_summary.bits_per_second(summary_receiving_stream.bits_per_second);
            receiving_summary.sender(summary_receiving_stream.sender);
            summaries.receiving_summary(receiving_summary);
            end_report.summaries(summaries);
            Iperf::ResourceStatistics cpu_usage;
            cpu_usage.host_total(result->end_report().cpu_usage.host_total);
            cpu_usage.host_user(result->end_report().cpu_usage.host_user);
            cpu_usage.host_system(result->end_report().cpu_usage.host_system);
            cpu_usage.remote_total(result->end_report().cpu_usage.remote_total);
            cpu_usage.remote_user(result->end_report().cpu_usage.remote_user);
            cpu_usage.remote_system(result->end_report().cpu_usage.remote_system);
            end_report.cpu_usage(cpu_usage);
            server_entity.end_report(end_report);

            servers.push_back(server_entity);
        }
        _message_structure.servers(servers);

        // Get clients information.
        std::vector<Iperf::IperfEntity> clients;
        for (const auto& [client_id, client] : *client_entities) {
            Iperf::IperfEntity client_entity;
            const iperf::Result* result;
            try {
                result = iperf::get_results(client);
            } catch (const iperf::Exception& e) {
                continue;
            }

            // Define header.
            Iperf::Header header;
            header.cookie(result->header().cookie);
            Iperf::Connection connection_item;
            std::vector<Iperf::ConnectionStatus> connections;
            for (const auto& connection : result->header().connection.connections) {
                Iperf::ConnectionStatus connection_status;
                connection_status.socket_number(connection.socket_number);
                connection_status.local_host(connection.local_host);
                connection_status.local_port(connection.local_port);
                connection_status.remote_host(connection.remote_host);
                connection_status.remote_port(connection.remote_port);
                connections.push_back(connection_status);
            }
            connection_item.connections(connections);
            Iperf::ConnectionIntention intention;
            intention.host(result->header().connection.intention.host);
            intention.port(result->header().connection.intention.port);
            connection_item.intention(intention);
            header.connection_item(connection_item);
            Iperf::BootstrapInformation information;
            information.version(result->header().information.version);
            information.system_information(result->header().information.system_information);
            information.tcp_mss_default(result->header().information.tcp_mss_default);
            information.target_bitrate(result->header().information.target_bitrate);
            information.sock_bufsize(result->header().information.sock_bufsize);
            information.sndbuf_actual(result->header().information.sndbuf_actual);
            information.rcvbuf_actual(result->header().information.rcvbuf_actual);
            information.protocol(result->header().information.protocol == iperf::TCP ? Iperf::ProtocolType::TCP : Iperf::ProtocolType::UDP);
            information.number_of_streams(result->header().information.number_of_streams);
            information.block_size(result->header().information.block_size);
            information.omitted_seconds(result->header().information.omitted_seconds);
            information.duration(result->header().information.duration);
            information.bytes(result->header().information.bytes);
            information.blocks(result->header().information.blocks);
            information.reverse_mode(result->header().information.reverse_mode);
            information.tos(result->header().information.tos);
            header.information(information);
            header.starting_time(result->header().starting_time.time_since_epoch().count());
            client_entity.header_item(header);

            // Set intervals.
            std::vector<Iperf::Interval> intervals;
            for (const auto& interval_item : result->intervals()) {
                Iperf::Interval interval;
                std::vector<Iperf::Stream> streams;
                for (const auto& stream : interval_item.streams) {
                    Iperf::Stream stream_item;
                    stream_item.socket_number(stream.socket_number);
                    stream_item.start(stream.start);
                    stream_item.end(stream.end);
                    stream_item.range(stream.range);
                    stream_item.bytes(stream.bytes);
                    stream_item.bits_per_second(stream.bits_per_second);
                    stream_item.omitted(stream.omitted);
                    stream_item.sender(stream.sender);
                    streams.push_back(stream_item);
                }
                interval.streams(streams);
                intervals.push_back(interval);
            }
            client_entity.intervals(intervals);

            // Set cumulative intervals.
            std::vector<Iperf::CumulativeInterval> cumulative_intervals;
            for (const auto& interval_item : result->cumulative_intervals()) {
                Iperf::CumulativeInterval cumulative_interval;
                cumulative_interval.start(interval_item.start);
                cumulative_interval.end(interval_item.end);
                cumulative_interval.range(interval_item.range);
                cumulative_interval.bytes(interval_item.bytes);
                cumulative_interval.bits_per_second(interval_item.bits_per_second);
                cumulative_intervals.push_back(cumulative_interval);
            }
            client_entity.cumulative_intervals(cumulative_intervals);

            // Add report data.
            Iperf::Report end_report;
            std::vector<Iperf::ReportedStreamsPair> streams;
            for (const auto& [sending_stream_item, receiving_stream_item] : result->end_report().streams) {
                Iperf::ReportedStreamsPair reported_pair;
                Iperf::ReportedSendingStream sending_stream;
                sending_stream.socket_number(sending_stream_item.socket_number);
                sending_stream.start(sending_stream_item.start);
                sending_stream.end(sending_stream_item.end);
                sending_stream.range(sending_stream_item.range);
                sending_stream.bytes(sending_stream_item.bytes);
                sending_stream.bits_per_second(sending_stream_item.bits_per_second);
                sending_stream.sender(sending_stream_item.sender);
                reported_pair.sending_stream(sending_stream);
                Iperf::ReportedReceivingStream receiving_stream;
                receiving_stream.socket_number(receiving_stream_item.socket_number);
                receiving_stream.start(receiving_stream_item.start);
                receiving_stream.end(receiving_stream_item.end);
                receiving_stream.range(receiving_stream_item.range);
                receiving_stream.bytes(receiving_stream_item.bytes);
                receiving_stream.bits_per_second(receiving_stream_item.bits_per_second);
                receiving_stream.sender(receiving_stream_item.sender);
                reported_pair.receiving_stream(receiving_stream);
                streams.push_back(reported_pair);
            }
            end_report.streams(streams);
            Iperf::ReportedSummariesPair summaries;
            auto [summary_sending_stream, summary_receiving_stream] = result->end_report().stream_summary;
            Iperf::ReportedSendingSummary sending_summary;
            sending_summary.start(summary_sending_stream.start);
            sending_summary.end(summary_sending_stream.end);
            sending_summary.range(summary_sending_stream.range);
            sending_summary.bytes(summary_sending_stream.bytes);
            sending_summary.bits_per_second(summary_sending_stream.bits_per_second);
            sending_summary.sender(summary_sending_stream.sender);
            summaries.sending_summary(sending_summary);
            Iperf::ReportedReceivingSummary receiving_summary;
            receiving_summary.start(summary_receiving_stream.start);
            receiving_summary.end(summary_receiving_stream.end);
            receiving_summary.range(summary_receiving_stream.range);
            receiving_summary.bytes(summary_receiving_stream.bytes);
            receiving_summary.bits_per_second(summary_receiving_stream.bits_per_second);
            receiving_summary.sender(summary_receiving_stream.sender);
            summaries.receiving_summary(receiving_summary);
            end_report.summaries(summaries);
            Iperf::ResourceStatistics cpu_usage;
            cpu_usage.host_total(result->end_report().cpu_usage.host_total);
            cpu_usage.host_user(result->end_report().cpu_usage.host_user);
            cpu_usage.host_system(result->end_report().cpu_usage.host_system);
            cpu_usage.remote_total(result->end_report().cpu_usage.remote_total);
            cpu_usage.remote_user(result->end_report().cpu_usage.remote_user);
            cpu_usage.remote_system(result->end_report().cpu_usage.remote_system);
            end_report.cpu_usage(cpu_usage);
            client_entity.end_report(end_report);

            clients.push_back(client_entity);
        }
        _message_structure.clients(clients);

    }

    bool execute() {
        // Define the participant-level entity.
        DomainParticipantQos participant_qos;
        participant_qos.name("Iperf_publisher");
        _participant = DomainParticipantFactory::get_instance()->create_participant(0, participant_qos);
        if (_participant == nullptr) {
            // TODO Handle this error.
            return false;
        }

        // Crate the publication topic.
        _topic = _participant->create_topic("yanpit/iperf", "YanpitIperf", TOPIC_QOS_DEFAULT);
        if (_topic == nullptr) {
            // TODO Handle this error.
            return false;
        }

        // Create the publisher entity.
        _publisher = _participant->create_publisher(PUBLISHER_QOS_DEFAULT);
        if (_publisher == nullptr) {
            // TODO Handle this error.
            return false;
        }

        // Set up the writer to perform publications.
        _writer = _publisher->create_datawriter(_topic, DATAWRITER_QOS_DEFAULT, &_listener);
        if (_writer == nullptr) {
            // TODO Handle this error.
            return false;
        }

        // Start the class inner thread.
        _is_running = true;
        _thread = std::thread(std::ref(*this));

        return true;
    }

    void operator()() {
        while (_is_running) {

            if (_listener._matched > 0) {
                _writer->write(&_message_structure);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    void stop() {
        if (!_is_running) {
            return;
        }
        _is_running = false;
        _thread.join();
    }

private:
    std::atomic<bool> _is_running = false;
    std::thread _thread;
    Iperf::IperfStatus _message_structure;
    DomainParticipant* _participant;
    Publisher* _publisher;
    Topic* _topic;
    DataWriter* _writer;
    IperfPubListener _listener;
};

#endif  // YANP_IT_DDS_IPERF_H
