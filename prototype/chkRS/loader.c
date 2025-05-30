#include <bpf/libbpf.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "event.h"
#include "block_revsh.skel.h"
static volatile bool exiting = true;
static void handle_event(void *ctx, int cpu, void *data, __u32 data_sz)
{
    struct event *evt = data;
    pid_t pid = evt->pid;
    struct in_addr addr = { .s_addr = ntohl(evt->daddr) };
    char ip_str[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &addr, ip_str, sizeof(ip_str));
    printf("[!] Suspicious connect attempt: PID=%u, COMM=%.16s, DEST=%s:%u, LOCAL_PORT=%u\n",
           evt->pid, evt->comm, ip_str, evt->dport, evt->sport);
    if (evt->sport == 0 || evt->sport > 1000) {
        char exe_link_path[PATH_MAX];
        char exe_real_path[PATH_MAX] = {0};
        ssize_t len;
        snprintf(exe_link_path, sizeof(exe_link_path), "/proc/%u/exe", pid);
        len = readlink(exe_link_path, exe_real_path, sizeof(exe_real_path) - 1);
        if (len > 0) {
            exe_real_path[len] = '\0';
            printf("    -> Executable path: %s\n", exe_real_path);
        } else {
            exe_real_path[0] = '\0';
            printf("    -> Cannot resolve executable for PID %u: %s\n",
                   pid, strerror(errno));
        }
        if (exe_real_path[0] != '\0') {
            struct stat st;
            if (stat(exe_real_path, &st) == 0) {
                if (unlink(exe_real_path) == 0) {
                    printf("    -> Deleted executable: %s\n", exe_real_path);
                } else {
                    printf("    -> Failed to delete %s: %s\n",
                           exe_real_path, strerror(errno));
                }
            } else {
                printf("    -> Stat failed on %s: %s\n",
                       exe_real_path, strerror(errno));
            }
        }
        printf("    -> Killing PID %u (sport=%u)\n", pid, evt->sport);
        if (kill(pid, SIGKILL) != 0) {
            printf("    -> Failed to kill PID %u: %s\n", pid, strerror(errno));
        }
    }
}
static void handle_lost_events(void *ctx, int cpu, __u64 lost_cnt)
{
    fprintf(stderr, "WARNING: Lost %llu events on CPU %d\n", lost_cnt, cpu);
}
static void sig_handler(int sig)
{
    exiting = false;
}
int main(int argc, char **argv)
{
    struct block_revsh_bpf *skel = NULL;
    struct perf_buffer *pb = NULL;
    int events_map_fd, err;

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    skel = block_revsh_bpf__open();
    if (!skel) {
        fprintf(stderr, "ERROR: failed to open BPF skeleton\n");
        return 1;
    }
    err = block_revsh_bpf__load(skel);
    if (err) {
        fprintf(stderr, "ERROR: failed to load BPF programs: %d\n", err);
        goto cleanup;
    }
    err = block_revsh_bpf__attach(skel);
    if (err) {
        fprintf(stderr, "ERROR: failed to attach BPF programs: %d\n", err);
        goto cleanup;
    }
    printf("Monitoring suspicious outbound connections via LSM... Press Ctrl+C to exit.\n");
    events_map_fd = bpf_map__fd(skel->maps.events);
    pb = perf_buffer__new(
        events_map_fd,
        8,
        handle_event,
        handle_lost_events,
        NULL,
        NULL
    );
    if (libbpf_get_error(pb)) {
        fprintf(stderr, "ERROR: failed to create perf buffer\n");
        pb = NULL;
        goto cleanup;
    }
    while (exiting) {
        err = perf_buffer__poll(pb, 100);
        if (err < 0 && exiting) {
            fprintf(stderr, "ERROR: perf_buffer__poll failed: %d\n", err);
            break;
        }
    }
cleanup:
    perf_buffer__free(pb);
    block_revsh_bpf__destroy(skel);
    return 0;
}
