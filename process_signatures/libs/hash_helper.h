#ifndef H_HASH_HELPERS
#define H_HASH_HELPERS

#include <linux/fs.h>
#include <linux/path.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <crypto/hash.h>
#include <linux/sched.h>
#include <linux/limits.h>
#include <linux/ktime.h>
#include <linux/time64.h>
#include <linux/mm.h>
#include <linux/mm_types.h>

void save_hash_snapshot(char* filename);

#endif /* H_HASH_HELPERS */
