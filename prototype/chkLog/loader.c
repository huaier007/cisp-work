#include <stdio.h>
#include <bpf/libbpf.h>
#include "chkLog.skel.h"
#include <unistd.h>

int main()
{
    struct chkLog_bpf *skel = chkLog_bpf__open_and_load();
    if (!skel) return 1;
    if (chkLog_bpf__attach(skel)) { chkLog_bpf__destroy(skel); return 2; }
    printf("Attached. Ctrl+C to exit.\n");
    while (1) sleep(1);
    return 0;
}
