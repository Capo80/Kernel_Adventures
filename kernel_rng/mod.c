#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <crypto/rng.h>

static __init int test_init(void) {

	pr_info("module init\n");

	struct crypto_rng * rng_handle = crypto_alloc_rng("ansi_cprng",  0, 0);

	char seed[32] = "kkzzppaa89abcdefzaybxcwdveuftgsh";
	if (crypto_rng_reset(rng_handle, (unsigned char*) &seed, 32) < 0) {
		printk("Failed reseed\n");
		return -1;
	}

	int output;
	crypto_rng_generate(rng_handle, NULL, 0, (unsigned char*) &output, 4);

	printk("rng: %d\n", output);

	crypto_rng_generate(rng_handle, NULL, 0, (unsigned char*) &output, 4);
	printk("rng: %d\n", output);

	crypto_rng_generate(rng_handle, NULL, 0, (unsigned char*) &output, 4);
	printk("rng: %d\n", output);

	crypto_free_rng(rng_handle);

	return 0;
}

void __exit test_exit(void) {


	pr_info("module exit\n");

}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pasquale Caporaso");
