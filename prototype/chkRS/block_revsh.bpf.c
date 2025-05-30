#ifndef __TARGET_ARCH_x86
#define __TARGET_ARCH_x86 1
#endif

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_endian.h>
#include "event.h"

#ifndef AF_INET
#define AF_INET 2
#endif

char LICENSE[] SEC("license") = "GPL";

struct {
    __uint(type, BPF_MAP_TYPE_PERF_EVENT_ARRAY);
    __uint(key_size, sizeof(__u32));
    __uint(value_size, sizeof(__u32));
} events SEC(".maps");

SEC("lsm/socket_connect")
int BPF_PROG(block_reverse_shell, struct socket *sock, struct sockaddr *addr, int addrlen)
{
    if (!addr || addrlen < sizeof(struct sockaddr_in))
        return 0;

    struct sockaddr_in sa4 = {};
    if (bpf_core_read(&sa4, sizeof(sa4), addr) != 0)
        return 0;

    if (sa4.sin_family != AF_INET)
        return 0;

    __u32 daddr = sa4.sin_addr.s_addr;
    __u16 dport = bpf_ntohs(sa4.sin_port);

    struct sock *sk = BPF_CORE_READ(sock, sk);
    if (!sk)
        return 0;

    __u16 sport = 0;
    bpf_core_read(&sport, sizeof(sport), &sk->__sk_common.skc_num);

    if (dport >= 0) {
        struct event e = {};
        __u32 pid = bpf_get_current_pid_tgid() >> 32;
        e.pid = pid;
        bpf_get_current_comm(&e.comm, sizeof(e.comm));

        e.daddr = daddr;
        e.dport = dport;
        e.sport = sport;

        bpf_perf_event_output(ctx, &events, BPF_F_CURRENT_CPU, &e, sizeof(e));
    }
    return 0;
}
