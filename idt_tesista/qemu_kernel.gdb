set history save on
set print pretty on

file /home/user/Desktop/compiling_kernel/linux-5.15.65/vmlinux

target remote :1234

set substitute-path /build/linux-zX5jE0/linux-5.13.0 /home/user/Desktop/compiling_kernel/linux-5.15.65

add-auto-load-safe-path /home/user/Desktop/compiling_kernel/linux-5.15.65/scripts/gdb/vmlinux-gdb.py