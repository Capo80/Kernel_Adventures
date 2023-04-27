#ifndef H_STRING_STEAL
#define H_STRING_STEAL

#include <linux/kprobes.h>

extern char* stolen_user_address;

extern struct mutex wait_steal_mutex;
extern spinlock_t wait_load_lock;
extern wait_queue_head_t wait_steal_queue;

int init_probe(void);
void remove_probe(void);

#endif /* H_STRING_STEAL */