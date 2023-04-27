#include "trap_controller.h"

// list of traced processes
struct h_node {
    pid_t tracee_pid;
    struct hlist_node node;
};

DEFINE_HASHTABLE(tracee_table, 3); //2**3 buckets, random for now
DECLARE_WAIT_QUEUE_HEAD(trap_wq);
bool tracer_running;
struct task_struct* trap_handler_ts = NULL;

static int trap_handler(struct notifier_block *nb, unsigned long val, void *args)
{
    struct die_args *die_args = args;
    struct h_node *cur;
    
    pr_info("trap handler activated: %d\n", current->pid);

    pr_info("Context level %lx\n",  preempt_count());
    
    // TODO: concurrency checks needed
    hash_for_each_possible(tracee_table, cur, node, current->pid) {
        pr_info("tracee process is trapping, stating syscall...\n");
        kernel_do_syscall_64(die_args->regs);
        return NOTIFY_STOP;
    }

    pr_info("current process is not traced\n");
    return NOTIFY_DONE;
}

static struct notifier_block trap_nb = {
	 .notifier_call = trap_handler,
};

//assume child is stopped
void trace_task(struct task_struct* ts) {

    struct h_node* new_tracee = kmalloc(sizeof(struct h_node), GFP_KERNEL);
    new_tracee->tracee_pid = ts->pid;

    hash_add(tracee_table, &new_tracee->node, ts->pid);

    wake_up_process(ts);

}

int setup_tracer(void) {

    return register_die_notifier(&trap_nb);

}

int teardown_tracer(void) {
    
    return unregister_die_notifier(&trap_nb);

}


