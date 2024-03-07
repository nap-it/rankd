#ifndef RANKD_PARSING_HELPERS_BPF_H
#define RANKD_PARSING_HELPERS_BPF_H

#include <linux/bpf.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/pkt_cls.h>
#include <bpf/bpf_endian.h>
#include <bpf/bpf_helpers.h>

#ifndef RANK_ETHERTYPE
#define RANK_ETHERTYPE 0x7265
#endif

static __always_inline int is_rank_frame(struct __sk_buff* sk_buff) {
    void* data = (void*)(long)sk_buff->data;
    void* data_end = (void*)(long)sk_buff->data_end;

    struct ethhdr* eth = data;

    if (data + sizeof(struct ethhdr) > data_end) {
        return 0;
    }

    if (bpf_ntohs(eth->h_proto) == RANK_ETHERTYPE) {
        return 1;
    } else {
        return 0;
    }
}

#endif //RANKD_PARSING_HELPERS_BPF_H
