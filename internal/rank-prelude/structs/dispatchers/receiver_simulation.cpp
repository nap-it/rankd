#include "structs/dispatchers/receiver_simulation.h"

std::pair<std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> *, std::mutex *>
RawReceiverSimulation::queue_access() {
    return std::make_pair(_queue, _queue_mutex);
}

RawReceiverSimulation *RawReceiverSimulation::receive_control_borrowing_from(ReceiverSimulation *controller) {
    _receiver_controller = controller;

    return this;
}

RawReceiverSimulation *RawReceiverSimulation::execute() {
    _logger->trace("[RawReceiverSimulation] Executing a RawReceiverSimulation...");
    if (_running) {
        _logger->warn("[RawReceiverSimulation] A raw receiver for simulation was already running. Someone is calling for execution again.");
        return this;
    }

    _logger->trace("[RawReceiverSimulation] Starting the main thread...");
    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

RawReceiverSimulation *RawReceiverSimulation::stop() {
    _logger->trace("[RawReceiverSimulation] Stopping a RawReceiverSimulation...");
    if (not _running) {
        _logger->warn("[RawReceiverSimulation] A raw receiver for simulation was already stopped. Someone is calling to stop again.");
        return this;
    }

    _logger->trace("[RawReceiverSimulation] Stopping the main thread...");
    _running = false;
    _thread.join();

    _logger->info("The raw receiver for simulation has been stopped.");

    return this;
}

bool RawReceiverSimulation::is_running() {
    return _running;
}

RawReceiverSimulation::RawReceiverSimulation() {
    _logger->trace("[RawReceiverSimulation] Preparing the simulation raw receiver...");
    _logger->trace("[RawReceiverSimulation] The simulation raw receiver is ready to run.");
}

RawReceiverSimulation::~RawReceiverSimulation() {}

void RawReceiverSimulation::operator()() {
    _logger->info("Starting the raw receiver for simulation main thread...");
    _logger->info("The raw receiver for simulation is now waiting for messages from other simulated nodes in blocking manner.");

    while (_running) {
        // Receive a message from the recv function.
        std::pair<uint8_t, std::vector<uint8_t>> data{};
        _logger->trace("[RawReceiverSimulation] Waiting for a message to come…");
        data = _simulated_recv();

        // If the data response is empty, ignore it.
        if (data.second.empty()) {
            _logger->error("[RawReceiverSimulation] The received message came without any data at all. We will ignore this message.");
            continue;
        }

        // Get origin from the first item in response.
        std::vector<uint8_t> source_identifier = {data.first};

        // Add such data to the queue to be handled.
        _logger->trace("[RawReceiverSimulation] Adding the received data onto the queue to be handled by the receiver for simulation.");
        std::lock_guard<std::mutex> guard(*_queue_mutex);
        _queue->emplace(source_identifier, data.second);

        // Execute the Receiver to handle such messages.
        _logger->trace("[RawReceiverSimulation] Awakening the receiver for simulation to handle these data.");
        _receiver_controller->execute();
    }
}

#ifdef FROM_SIMUZILLA
RawReceiverSimulation *
RawReceiverSimulation::borrow_receiver_function(std::function<std::pair<uint8_t, std::vector<uint8_t>>(void)>& function) {
    _logger->trace("[RawReceiverSimulation] Registering Rx function from Simuzilla.");
    _simulated_recv = function;

    return this;
}
#endif

void ReceiverSimulation::set_queue(std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> *queue, std::mutex *mutex) {
    _queue = queue;
    _queue_mutex = mutex;
}

void ReceiverSimulation::set_message_deposit_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>> *queue, std::mutex *mutex) {
    _message_deposit = queue;
    _message_deposit_mutex = mutex;
}

ReceiverSimulation *ReceiverSimulation::execute() {
    _logger->trace("[ReceiverSimulation] Executing a ReceiverSimulation...");
    if (_running) {
        _logger->warn("[ReceiverSimulation] A receiver for simulation was already running. Someone is calling for execution again.");
        return this;
    }

    _logger->trace("[ReceiverSimulation] Starting the main thread...");
    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

ReceiverSimulation *ReceiverSimulation::stop() {
    _logger->trace("[ReceiverSimulation] Stopping a ReceiverSimulation...");
    if (not _running) {
        _logger->warn("[ReceiverSimulation] A raw receiver for simulation was already stopped. Someone is calling to stop again.");
        return this;
    }

    _logger->trace("[ReceiverSimulation] Stopping the main thread...");
    _running = false;
    _thread.join();

    _logger->info("The receiver for simulation has been stopped.");

    return this;
}

bool ReceiverSimulation::is_running() {
    return _running;
}

void ReceiverSimulation::operator()() {
    _logger->info("The receiver for simulation was awakened to handle messages that were deposited in the queue.");

    while (_running) {
        if (not _queue->empty()) {
            _logger->trace("[ReceiverSimulation] The queue still has an item to be handled (a total of {} items).", _queue->size());
            std::pair<std::vector<uint8_t>, std::vector<uint8_t>> source_and_bytes;
            {
                std::lock_guard<std::mutex> guard(*_queue_mutex);
                source_and_bytes = _queue->front();
                _queue->pop();
            }
            _logger->trace("[ReceiverSimulation] Received a source with {} bytes, and {} message bytes.", source_and_bytes.first.size(), source_and_bytes.second.size());

            Message* message = parse_message_from_bytes(source_and_bytes.second, false);
            _logger->debug("[ReceiverSimulation] Bytes --> Message(bytes): {} bytes into a {} message.", source_and_bytes.second.size(), (message != nullptr ? "parseable" : "non-parseable"));
            if (message == nullptr) {
                // TODO Handle this error, since it means that this message will be ignored.
                _logger->error("[ReceiverSimulation] Ignoring erroneous message as it was parsed out from data bytes.");
                continue;
            }
            _logger->trace("[ReceiverSimulation] Enqueueing the parsed message into the depositing queue of the dispatcher.");
            {
                std::lock_guard guard(*_message_deposit_mutex);
                _message_deposit->emplace(message, source_and_bytes.first, IdentifierType::Simulation);
            }
            _logger->info("The receiver for simulation have just enqueued a message into the Dispatcher's depositing queue.");
        } else {
            _logger->trace("[ReceiverSimulation] The receiver for simulation does not have any item to handle in its queue. Stopping function.");
            stop();
            _logger->info("The receiver for simulation has now ceased functions as there is no data waiting to be handled.");
        }
    }
}