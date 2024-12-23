#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define SHM_SIZE	4096  // Size of the shared memory segment
#define NOP		0xC3  // RET instruction byte (x86 architecture)
#define KEY_FILE	"/tmp/test_shared"

int main() {
    key_t key;
    int shmid;
    char *shmaddr;

    // Generate a unique key for the shared memory segment
    key = ftok(KEY_FILE, 'R');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // Allocate a shared memory segment
    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    // Attach the shared memory segment to the process's address space
    shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    printf("SHADDR: %lx\n", shmaddr);
    getchar();
    // Fill the shared memory segment with NOP instructions
    memset(shmaddr, NOP, SHM_SIZE);

    printf("Shared memory segment created and filled with NOPs.\n");
    printf("Shared memory ID: %d\n", shmid);
    printf("First few bytes in shared memory: ");
    for (int i = 0; i < 10; ++i) {
        printf("%02X ", (unsigned char)shmaddr[i]);
    }
    printf("\n");

    getchar();

    // Detach from the shared memory segment
    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(1);
    }

    // Optionally, mark the shared memory segment for deletion
    // Uncomment the following lines to delete the segment:
    // if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    //     perror("shmctl");
    //     exit(1);
    // }

    return 0;
}
