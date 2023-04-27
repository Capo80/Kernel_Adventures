#ifndef H_PROBE_HELPERS
#define H_PROBE_HELPERS

#include <linux/kprobes.h>
#include <linux/slab.h>
#include <linux/dcache.h>
#include <linux/sched.h>
#include <linux/path.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <crypto/hash.h>



struct process_info {

    int vm_area_number;
    char *pathname;
    struct list_head vm_area_list;

    struct list_head list;

};

struct vm_area_info {

    unsigned long size;
    char* path;
    char* md5;

    struct list_head list;
};
struct process_info;
extern struct list_head process_info_list;

int init_probe(void);
void remove_probe(void);

#endif /* H_PROBE_HELPERS */