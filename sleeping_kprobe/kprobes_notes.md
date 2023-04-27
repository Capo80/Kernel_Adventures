# Kprobe handler (kprobe_int3_handler)

## case STANDARD

set_current_probe:
- save kprobe struct to cpu;
- save (0 & interrupt enabled flags) to kcb->old_flags and kcb->saved_flags;

save KPROBE_HIT_ACTIVE to kcb->status; 

do pre_handler

setup_singlestep (reenter = 0):
- check if optimized (if yes, do nothing)
- check if boost yes, if yes:
    - reset kprobe struct in per_cpu ( = NULL)
    - exec overwriten istr directly where its written
    - return
- save KPROBE_HIT_SS to kcb->status;
- check if we can emulate_op:
    - emulate operation (change the registers based on the instruction?)
    - kprobe_post_process:
        - save KPROBE_HIT_SSDONE to kcb->status;
        - do post_handler
        - reset kprobe struct in per_cpu ( = NULL)
    - return
- disable interrupt
- set ip to trampoline
- return

## case HIT_SS

get kprobe_struct from cpu

restore saved interrupt flag

restore ip to after the int3

kprobe_post_process:
- save KPROBE_HIT_SSDONE to kcb->status;
- do post_handler
- reset kprobe struct in per_cpu ( = NULL)

## case REENTER

```
static int reenter_kprobe(struct kprobe *p, struct pt_regs *regs,
			  struct kprobe_ctlblk *kcb)
{
	switch (kcb->kprobe_status) {
	case KPROBE_HIT_SSDONE: // probe triggered in post handler
	case KPROBE_HIT_ACTIVE: // probe triggered in pre handler
    ... more stuff
    }

	return 1;
}
```
in both cases do nothing and do not trigger new probe

other cases:
- HIT_SS: (we hit a probe while handling a probe, we dont trigger the new one)
    - add 1 to nmissed
    - setup_singlestep (we need to fix the new probe hit) (reenter = 1):
        - save old kcb in current kcb
        - save current kprobe struct to cpu
        - save interrupt flags to to new kcb in cpu
        - save KPROBE_REENTER to kcb status
        - do single_step, same as before
- HIT_REENTER: (we hit a probe while handling a probe while handling a probe):
    - we fucked up -> KERNEL PANIC
