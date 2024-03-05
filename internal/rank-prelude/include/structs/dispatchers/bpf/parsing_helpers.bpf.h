#ifndef RANKD_PARSING_HELPERS_BPF_H
#define RANKD_PARSING_HELPERS_BPF_H

#include <stddef.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

/// Header cursor to keep track of current parsing position.
struct hdr_cursor {
    void* pos;
};

/// Struct representing a VLAN header.
/// @h_vlan_TCI priority and VLAN ID.
/// @h_vlan_encapsulation_proto packet type ID or length.
struct vlan_header {
    __be16 h_vlan_TCI;
    __be16 h_vlan_encapsulated_proto;
};

#ifndef VLAN_MAX_DEPTH
#define VLAN_MAX_DEPTH 2
#endif

#define VLAN_VID_MASK 0x0fff

static __always_inline int proto_is_vlan(__u16 header_proto) {
    return !!(header_proto == bpf_htons(ETH_P_8021Q) || header_proto == bpf_htons(ETH_P_8021AD));
}

static __always_inline int parse_ethernet_header_vlan(struct hdr_cursor* nh, void* data_end, struct ethhdr** ethhdr, struct collect_vlans* vlans) {
    struct ethhdr* ethernet = (struct ethhdr*) nh->pos;
    int header_size = sizeof(*ethernet);
    struct vlan_header* v_header;
    __u16 header_protocol;
    int i;

    if (nh->pos + header_size > data_end) {
        return -1;
    }

    nh->pos += header_size;
    *ethhdr = ethernet;
    v_header = nh->pos;
    header_protocol = ethernet->h_proto;

    #pragma unroll
    for (i = 0; i < VLAN_MAX_DEPTH; i++) {
        if (!proto_is_vlan(header_protocol)) {
            break;
        }
        if (v_header + 1 > data_end) {
            break;
        }
        header_protocol = v_header->h_vlan_encapsulated_proto;
        if (vlans) {
            vlans->id[i] = (bpf_ntons(v_header->h_vlan_TCI) & VLAN_VID_MASK);
        }
        v_header++;
    }

    nh->pos = v_header;
    return header_protocol;
}

static __always_inline int parse_ethernet_header(struct hdr_cursor* nh, void* data_end, struct etthdr** ethhdr) {
    return parse_ethernet_header_vlan(nh, data_end, ethhdr, NULL);
}

#endif //RANKD_PARSING_HELPERS_BPF_H
