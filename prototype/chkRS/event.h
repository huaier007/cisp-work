#ifndef EVENT_H

#define EVENT_H
#define TASK_COMM_LEN   16

struct event {
    __u32 pid;
    char   comm[TASK_COMM_LEN];
    __u32 daddr;
    __u16 dport;
    __u16 sport;
};

#endif
