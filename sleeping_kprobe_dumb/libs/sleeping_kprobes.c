#include "sleeping_kprobes.h"

DEFINE_PER_CPU(unsigned long, BRUTE_START);
DEFINE_PER_CPU(unsigned long*, current_kprobe_cpu_addr) = NULL;


/**
 * @brief find first closest address fro mthe bottom in cpu memory that contains a 64bit values
 * 
 * @param value to search 
 * @return unsigned long, first address containing the value 
 */
unsigned long* find_cpu_var(unsigned long value) {
	
	unsigned long* temp = (unsigned long) &BRUTE_START;
	while (temp > 0) {
		temp -= 1; 
		//printk("kprobe: %lx\n", (unsigned long) temp);
		//printk("value: %lx - %lx\n", (unsigned long) temp, __this_cpu_read(*temp));
		if ((unsigned long) __this_cpu_read(*temp) == value) {
			//printk("value: %lx - %lx\n", (unsigned long) temp, __this_cpu_read(*temp));
			break;
		}
	}

	if (temp == 0) {
		return NULL;
	}

	return temp;
}

/**
 * @brief header for a sleeping kprobe, call this before sleeping in a kprobe,
 *        it will search and NULL the current_krpobe cpu variable
 * 
 * @param ri 
 * @return int < 0 fail, 0 success 
 */
int kprobe_sleep_header(struct kprobe *ri) {

    unsigned long* kprobe_cpu = __this_cpu_read(current_kprobe_cpu_addr);

    //check if we already searched
    if (kprobe_cpu == NULL) {
        kprobe_cpu = find_cpu_var((unsigned long) ri);
        if (kprobe_cpu == NULL) {
            pr_err("FATAL! Cannot find cpu var");
            return -1;
        }
        __this_cpu_write(current_kprobe_cpu_addr, kprobe_cpu); // save after the search
    }

	__this_cpu_write(*kprobe_cpu, NULL);
	preempt_enable();
    return 0;

}

/**
 * @brief footer for a sleeping kprobe, call this after the sleep
 *        it will restore the CPU var
 * 
 * @param ri 
 * @return int < 0 fail, 0 success  
 */
int kprobe_sleep_footer(struct kprobe *ri) {

    // this can be null if we landed on a different cpu not yet searched
    unsigned long* kprobe_cpu = __this_cpu_read(current_kprobe_cpu_addr);
    if (kprobe_cpu == NULL) {
        kprobe_cpu = find_cpu_var((unsigned long) ri);
        if (kprobe_cpu == NULL) {
            pr_err("FATAL! Cannot find cpu var");
            return -1;
        }
        __this_cpu_write(current_kprobe_cpu_addr, kprobe_cpu); // save after the search
    }
	preempt_disable();
	__this_cpu_write(*kprobe_cpu, ri);

    return 0;
}


/**
 * @brief header for a sleeping kretprobe, call this before sleeping in a kprobe,
 *        it will search and NULL the current_krpobe cpu variable
 * 
 * @param ri 
 * @return int < 0 fail, 0 success 
 */
int kretprobe_sleep_header(struct kretprobe_instance *ri) {
    
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,11,0)
	struct kretprobe *rp = ri->rph->rp;
#else
	struct kretprobe *rp = ri->rp;
#endif

    unsigned long* kprobe_cpu = __this_cpu_read(current_kprobe_cpu_addr);

    //check if we already searched
    if (kprobe_cpu == NULL) {
        pr_info("SEARCHING...");
        kprobe_cpu = find_cpu_var((unsigned long) &rp->kp);
        if (kprobe_cpu == NULL) {
            pr_err("FATAL! Cannot find cpu var");
            return -1;
        }
        __this_cpu_write(current_kprobe_cpu_addr, kprobe_cpu); // save after the search
    }


	__this_cpu_write(*kprobe_cpu, NULL);
	preempt_enable();
    return 0;

}

/**
 * @brief footer for a sleeping kretprobe, call this after the sleep
 *        it will restore the CPU var
 * 
 * @param ri 
 * @return int < 0 fail, 0 success  
 */
int kretprobe_sleep_footer(struct kretprobe_instance *ri) {
    
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,11,0)
	struct kretprobe *rp = ri->rph->rp;
#else
	struct kretprobe *rp = ri->rp;
#endif

    // this can be null if we landed on a different cpu not yet searched
    unsigned long* kprobe_cpu = __this_cpu_read(current_kprobe_cpu_addr);
    if (kprobe_cpu == NULL) {
        pr_info("SEARCHING...");
        kprobe_cpu = find_cpu_var((unsigned long) &rp->kp);
        if (kprobe_cpu == NULL) {
            pr_err("FATAL! Cannot find cpu var");
            return -1;
        }
        __this_cpu_write(current_kprobe_cpu_addr, kprobe_cpu); // save after the search
    }
	preempt_disable();
	__this_cpu_write(*kprobe_cpu, ri);

    return 0;
}

