#include "data-sources/yanpit.h"

YANPit* YANPit::execute() {
    _running = true;
    _thread = std::thread(std::ref(*this));

    return get_instance();
}

YANPit* YANPit::stop() {
    // 1. If it is already stopped, quit.
    if (!_running) {
        return get_instance();
    }

    // 2. Stop this instance.
    _running = false;
    _thread.join();

    return get_instance();
}

void YANPit::operator()() {
    while (_running) {
        // TODO Do something...

        std::this_thread::sleep_for(std::chrono::milliseconds(_thread_sampling_rate));
    }
}

YANPit::~YANPit() {
    stop();
}