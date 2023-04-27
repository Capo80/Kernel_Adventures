#include "load_helpers.h"

int copy_chunked_from_kernel(void *dst, const void *usrc, unsigned long len) {

    do {
		unsigned long n = min(len, COPY_CHUNK_SIZE);

		if (memcpy(dst, usrc, n) == NULL){
            printk(KERN_ERR "failed with len-err: %lu\n", n);
			return -EFAULT;
        }
        dst += n;
		usrc += n;
		len -= n;

	} while (len);

	return 0;

}

