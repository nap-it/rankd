#include "../../../include/structs/dispatchers/bpf/parsing_helpers.bpf.h"

struct {
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __uint(max_entries, 1);
    __type(key, int);
    __type(value, int);
} interface SEC(".maps");

static __u32 zero = 0;

SEC("classifier")
int tc_ingress_move_rank_frames(struct __sk_buff* sk_buff) {
    int interface_item;
    int* iface = bpf_map_lookup_elem(&interface, &zero);

    if (iface) {
        if (is_rank_frame(sk_buff) == 1) {
            return bpf_redirect(*iface, 0);
        }
    }

    return TC_ACT_OK;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";