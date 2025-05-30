#include <stdio.h>
#include <unistd.h>
#include <bpf/libbpf.h>
#include "chkRaW.skel.h"

int main(int argc, char **argv)
{
    struct chkRaW_bpf *skel;
    int err;
    skel = chkRaW_bpf__open_and_load();
    if (!skel) {
        fprintf(stderr, "Failed to open and load skeleton\n");
        return 1;
    }
    err = chkRaW_bpf__attach(skel);
    if (err) {
        fprintf(stderr, "Failed to attach programs: %d\n", err);
        chkRaW_bpf__destroy(skel);
        return 1;
    }
    printf("eBPF LSM programs attached. Blocking /etc/shadow access...\n");
    while (1)
        sleep(1);
    chkRaW_bpf__destroy(skel);
    return 0;
}
