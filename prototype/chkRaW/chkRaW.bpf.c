#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_tracing.h>
#include <linux/errno.h>

char LICENSE[] SEC("license") = "GPL";

static __always_inline bool is_sshd(void)
{
    char comm[TASK_COMM_LEN] = {};
    bpf_get_current_comm(&comm, sizeof(comm));
    if (comm[0] == 's' && comm[1] == 's' && comm[2] == 'h' && comm[3] == 'd')
        return true;
    return false;
}

SEC("lsm/file_open")
int BPF_PROG(deny_open_shadow, struct file *file)
{
    struct dentry *d = (struct dentry *)BPF_CORE_READ(file, f_path.dentry);
    if (!d)
        return 0;
    char fname[16] = {};
    const char *name_ptr = (const char *)BPF_CORE_READ(d, d_name.name);
    bpf_core_read_str(fname, sizeof(fname), name_ptr);
    if (__builtin_strcmp(fname, "shadow") != 0)
        return 0;
    struct dentry *parent = (struct dentry *)BPF_CORE_READ(d, d_parent);
    if (!parent)
        return 0;
    char parent_name[16] = {};
    const char *parent_ptr = (const char *)BPF_CORE_READ(parent, d_name.name);
    bpf_core_read_str(parent_name, sizeof(parent_name), parent_ptr);
    if (__builtin_strcmp(parent_name, "etc") != 0)
        return 0;
    if (!is_sshd()) {
        bpf_printk("deny_open_shadow: blocked /etc/shadow by non-sshd task\n");
        return -EPERM;
    }
    return 0;
}
