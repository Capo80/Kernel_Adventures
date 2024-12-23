#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>

#define SHM_SIZE 4096  // Size of the shared memory segment
#define KEY_FILE	"/tmp/test_shared"

int main() {
    key_t key;
    int shmid;
    void *shmaddr;

    // Generate the same unique key as the writer program
    key = ftok(KEY_FILE, 'R');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    getchar();

    // Retrieve the shared memory segment
    shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    getchar();

    // Attach the shared memory segment to the process's address space
    shmaddr = shmat(shmid, NULL, SHM_EXEC | SHM_RDONLY);
    if (shmaddr == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    
    // Mark the shared memory as executable
    /* if (mprotect(shmaddr, SHM_SIZE, PROT_READ | PROT_EXEC) == -1) { */
    /*     perror("mprotect"); */
    /*     exit(1); */
    /* } */

    printf("Executing NOPs from shared memory...\n");

    // Cast the shared memory to a function pointer and call it
    void (*func)() = (void (*)())shmaddr;
    func();

    getchar();
    printf("Execution finished.\n");

    // Detach from the shared memory segment
    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
