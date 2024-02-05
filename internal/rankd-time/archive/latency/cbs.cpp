#include "latency/cbs.h"

CBSEntry::CBSEntry(const struct rtattr *attribute) {
    auto *queue_value = (struct tc_cbs_qopt*) RTA_DATA(attribute);

    _hi_credit = queue_value->hicredit;
    _lo_credit = queue_value->locredit;
    _idle_slope = queue_value->idleslope;
    _send_slope = queue_value->sendslope;
    _offload = queue_value->offload;
}

#ifdef LINUX_TARGET

CBS::CBS() {
    // Request information on QDiscs to Linux's netlink and get vector of CBS entries.
    _netlink_socket.request_get_for_qdiscs();
    auto configured_cbs = _netlink_socket.qdiscs(CBSEntry::check_identity);

    // Clear late versions of entries.
    _entries.clear();

    // Create entries based on vector elements.
    for (const auto &[conf_attribute, _]: configured_cbs) {
        _entries.insert(CBSEntry(conf_attribute));
    }
}

#endif