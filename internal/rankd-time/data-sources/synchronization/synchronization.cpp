#include "data-sources/synchronization/synchronization.h"

Synchronization::Synchronization() {
    _output_type = OUTPUT_SYNC_TYPE_ALL;

}

void Synchronization::snap() {
#ifndef LINUX_PTP
    //snap_ptp_via_linux_ptp();
#elif defined(RELYUM)
    snap_ptp_via_relyum();
#endif
}

void Synchronization::enable_json_output() {
    _json_formatted_output = true;
}

void Synchronization::disable_json_output() {
    _json_formatted_output = false;
}

void Synchronization::set_output(uint8_t type) {
    _output_type = type;
}

rapidjson::Document Synchronization::json() const {
    // Create a JSON document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    // Create a temporary value.
    rapidjson::Value value;



    return json_document;
}