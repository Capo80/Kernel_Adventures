#define EXIT    0x3c
#define OPEN    0x02
#define WRITE   0x01

#define O_WRONLY             01
#define O_CREAT             0100


#define syscall_1(x, a, ret) \
    __asm volatile (                                                \
        "movq %1, %%rdi\n"                                          \
        "movq %2, %%rax\n"                                          \
        "syscall\n"                                                 \
        "movq %%rax, %0\n"                                          \
        : "=r" (*((unsigned long*)ret))                             \
        : "r" ((unsigned long)a), "N" ((unsigned long)x)            \
        : "rax", "rdi", "r10"                                       \
        );                                                          \


#define syscall_2(x, a, b, ret) \
    __asm volatile (                                                \
        "movq %1, %%rdi\n"                                          \
        "movq %2, %%rsi\n"                                          \
        "movq %3, %%rax\n"                                          \
        "syscall\n"                                                 \
        "movq %%rax, %0\n"                                          \
        : "=r" (*((unsigned long*)ret))                             \
        : "r" ((unsigned long)a), "r" ((unsigned long)b),           \
          "N" ((unsigned long)x)                                    \
        : "rax", "rdi" , "rsi"                                      \
        );                                                          \

#define syscall_3(x, a, b, c, ret) \
    __asm volatile (                                                \
        "movq %1, %%rdi\n"                                          \
        "movq %2, %%rsi\n"                                          \
        "movq %3, %%rdx\n"                                          \
        "movq %4, %%rax\n"                                          \
        "syscall\n"                                                 \
        "movq %%rax, %0\n"                                          \
        : "=r" (*((unsigned long*)ret))                             \
        : "r" ((unsigned long)a), "r" ((unsigned long)b),           \
          "r" ((unsigned long)c), "N" ((unsigned long)x)            \
        : "rax", "rdi" , "rsi" , "rdx"                              \
        );             


void _start(){

    unsigned long ret;
    unsigned long fd;
//    char path[] = "/home/user/test.txt";
//    char text[] = "hello";

    syscall_3(OPEN, "/home/user/test.txt", O_CREAT | O_WRONLY, 00777 , &fd);

    syscall_3(WRITE, fd, "hello", 6, &ret);

    syscall_1(EXIT, ret, &ret);
    
}