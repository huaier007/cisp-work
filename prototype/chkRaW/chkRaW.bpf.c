#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_tracing.h>
#include <linux/errno.h>

char LICENSE[] SEC("license") = "GPL";

SEC("lsm/file_open")
int BPF_PROG(deny_open_shadow, struct file *file)
{
    struct path *p = &file->f_path;
    struct dentry *d = p->dentry;
    char name[64];
    bpf_core_read_str(name, sizeof(name), d->d_name.name);
    if (__builtin_strcmp(name, "shadow") == 0) {
        struct path *parent_path = &p->mnt->mnt_root;
        bpf_printk("deny open /etc/shadow\n");
        return -EPERM;
    }
    return 0;
}
SEC("lsm/inode_permission")
int BPF_PROG(deny_perm_shadow, struct inode *inode, int mask)
{
    char path_name[64];
    bpf_core_read_str(path_name, sizeof(path_name), inode->i_sb->s_id);
    return 0;
}
