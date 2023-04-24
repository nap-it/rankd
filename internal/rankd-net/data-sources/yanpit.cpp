#include "data-sources/yanpit.h"

YANPit* YANPit::execute() {
    // 0. Check if YANPit already is writing on a broker or not.
    _yanpit_was_brokered_before_rankd = false;
    bool yanpit_is_found_on_system = false;

    auto request = std::make_shared<restbed::Request>(restbed::Uri(YANPIT_BROKER_STATUS_URI));
    request->set_header("Accept", "*/*");
    request->set_header("Host", "localhost");
    try {
        auto response = std::string(reinterpret_cast<const char*>(restbed::Http::sync(request)->get_body().data()));
        _yanpit_was_brokered_before_rankd = response == "ON";
    } catch (const std::exception& e) {
        return nullptr;
    }

    // 1. Set YANPit to write on broker and check if it is running or not.
    request = std::make_shared<restbed::Request>(restbed::Uri(YANPIT_BROKER_START_URI));
    request->set_header("Accept", "*/*");
    request->set_header("Host", "localhost");
    restbed::Http::sync(request);

    // 2. If it is running, set attributes accordingly and return instance.
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

    // 3. Revert broker status on YANPit.
    if (!_yanpit_was_brokered_before_rankd) {
        // Send signal to YANPit to stop communicating with broker.
        auto request = std::make_shared<restbed::Request>(restbed::Uri(YANPIT_BROKER_STOP_URI));
        request->set_header("Accept", "*/*");
        request->set_header("Host", "localhost");
        restbed::Http::sync(request);
    }

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