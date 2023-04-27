#include "symbol_lookup.h"

typedef unsigned long(*kallsymsFn)(const char *);

static kallsymsFn kallsyms = NULL;

unsigned long kallsyms_lookup_name(const char *name)
{
	return kallsyms(name);
}

int init_kallsyms(void)
{
	struct kprobe kp = {0};
	int ret = 0;
	kp.symbol_name = "kallsyms_lookup_name";

	ret = register_kprobe(&kp);

	if (ret < 0)
		return ret;

	kallsyms = (kallsymsFn)kp.addr;

	unregister_kprobe(&kp);

	return ret;
}