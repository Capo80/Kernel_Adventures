set history save on
set print pretty on

file /home/user/programs/noble/debian/build/build-generic/vmlinux

target remote :1234

set substitute-path /build/linux-zX5jE0/linux-5.13.0 /home/user/programs/noble

add-auto-load-safe-path /home/user/programs/noble

source /home/user/programs/noble/vmlinux-gdb.py