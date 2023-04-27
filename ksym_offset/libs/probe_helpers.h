#ifndef H_PROBE_HELPERS
#define H_PROBE_HELPERS

#include <linux/kprobes.h>
#include <linux/slab.h>

int init_probe(void);
void remove_probe(void);

#endif /* H_PROBE_HELPERS */