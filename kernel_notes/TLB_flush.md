# TLB flushing

TLB flushing is done with the "tlb_flush_*" functions, some are local cpu only some do one more than one cpu

We can use the "on_each_cpu*" function to make flush execute on all CPUs.

```

/*
 * on_each_cpu_cond(): Call a function on each processor for which
 * the supplied function cond_func returns true, optionally waiting
 * for all the required CPUs to finish. This may include the local
 * processor.
 * @cond_func:	A callback function that is passed a cpu id and
 *		the info parameter. The function is called
 *		with preemption disabled. The function should
 *		return a blooean value indicating whether to IPI
 *		the specified CPU.
 * @func:	The function to run on all applicable CPUs.
 *		This must be fast and non-blocking.
 * @info:	An arbitrary pointer to pass to both functions.
 * @wait:	If true, wait (atomically) until function has
 *		completed on other CPUs.
 *
 * Preemption is disabled to protect against CPUs going offline but not online.
 * CPUs going online during the call will not be seen or sent an IPI.
 *
 * You must not call this function with disabled interrupts or
 * from a hardware interrupt handler or from a bottom half handler.
 */
 https://elixir.bootlin.com/linux/latest/source/kernel/smp.c#L1003
```

for some of theese we may need a little stub to exctract the argument, something like this:
```
static void flush_tlb_one_ipi(void *info)
{
	struct flush_tlb_data *fd = (struct flush_tlb_data *)info;
	local_flush_tlb_one(fd->addr1, fd->addr2);
}
https://elixir.bootlin.com/linux/latest/source/arch/sh/kernel/smp.c#L454
```
