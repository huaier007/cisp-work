#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include <linux/errno.h>

char LICENSE[] SEC("license") = "GPL";

SEC("lsm/inode_unlink")
int BPF_PROG(deny_inode_unlink, struct inode *dir, struct dentry *dentry)
{
    char fname[64];
    bpf_core_read_str(fname, sizeof(fname), dentry->d_name.name);
    bpf_printk("inode_unlink %s\n", fname);
    if (!__builtin_strcmp(fname, "wtmp"))
        return -EPERM;
    return 0;
}

SEC("lsm/path_unlink")
int BPF_PROG(deny_path_unlink, struct path *parent, struct dentry *dentry)
{
    char fname[64], pname[64];
    struct dentry *pd = parent->dentry;
    bpf_core_read_str(pname, sizeof(pname), pd->d_name.name);
    bpf_core_read_str(fname, sizeof(fname), dentry->d_name.name);
    bpf_printk("path_unlink %s/%s\n", pname, fname);
    if (!__builtin_strcmp(pname, "log"))// &&
//        !__builtin_strcmp(fname, "wtmp"))
        return -EPERM;
    return 0;
}
