# QEMU + GDB + VIRT-MANAGER Graphical Kernel Debug

Prepared for:
```
No LSB modules are available.
Distributor ID: Ubuntu
Description:    Ubuntu 24.04 LTS
Release:        24.04
Codename:       noble

Linux computer 6.8.0-40-generic #40-Ubuntu SMP PREEMPT_DYNAMIC Fri Jul  5 10:34:03 UTC 2024 x86_64 x86_64 x86_64 GNU/Linux
```

## Install Qemu + Virt-Manager

Install 

```bash
sudo apt-get install qemu-system
sudo apt-get install virt-manager 
```

Start KVM libvirt service

```bash
sudo systemctl enable libvirtd.service
sudo systemctl start libvirtd.service

# Check that the service is running (press 'q' to quit):

systemctl status libvirtd.service
```

Allow low-level users to use KVM.

```bash
# Create group

newgrp libvirt

# Edit the /etc/libvirt/libvirtd.conf file with your preferred editor (nano is used in this example):

sudo nano /etc/libvirt/libvirtd.conf

# Navigate to around line 85 and uncomment the following lines:

#   unix_sock_group = "libvirt"

# And then navigate to around line 108 and uncomment:

#   unix_sock_rw_perms = "0770"

# Add your user to the libvirt group

sudo usermod -G libvirt -a `whoami`
```

Restart libvirt deamon:
```bash
sudo systemctl restart libvirtd.service
```

RELOG

## Install ubuntu LTS image

Download and install with Virt Manager as normal

## Compile Kernel

Add deb-src to source.list:
```
https://askubuntu.com/questions/496549/error-you-must-put-some-source-uris-in-your-sources-list
```

Install dependencies
```bash
sudo apt build-dep linux linux-image-unsigned-$(uname -r)
sudo apt install libncurses-dev gawk flex bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf llvm
```

Get ubuntu kernel from git repo:
```bash
git clone  git://git.launchpad.net/~ubuntu-kernel/ubuntu/+source/linux/+git/noble
```

Get version with uname -a from virtual machine:
```bash
uname -a
# Linux xubuntu 6.8.0-31-generic #31-Ubuntu SMP PREEMPT_DYNAMIC Sat Apr 20 00:40:06 UTC 2024 x86_64 x86_64 x86_64 GNU/Linux
```

Checkout correct kernel version (can check name with "git tag"):
```bash
git checkout tags/Ubuntu-6.8.0-31.31
```

Alternative, clone directly the desired branch:
```bash
git clone --depth 1 --branch Ubuntu-5.15.0-35.36+22.10.1 git://git.launchpad.net/~ubuntu-kernel/ubuntu/+source/linux/+git/noble
```
Generate config file
```bash
chmod a+x debian/rules
chmod a+x debian/scripts/*
chmod a+x debian/scripts/misc/*
fakeroot debian/rules clean
fakeroot debian/rules editconfigs # you need to go through each (Y, Exit, Y, Exit..) or get a complaint about config later1\
```

in "debian.master/config/annotations" change line 4 to this or pray to not get error:
```
# FLAVOUR: amd64-generic 
```

Config should outmatically have this set up
```
# CONFIG_DEBUG_RODATA is not set

CONFIG_FRAME_POINTER=y

CONFIG_KGDB=y

CONFIG_KGDB_SERIAL_CONSOLE=y
```


Then compile
```bash
fakeroot debian/rules binary
```

Check deb packages are present:
```bash
cd ..
ls -la *.deb
```

## Install kernel on VM

Install all on VM:
```bash
sudo dpkg -i ./*
```

Check place in grub:
```bash
sudo grub-mkconfig | grep -iE "menuentry 'Ubuntu, with Linux" | awk '{print i++ " : "$1, $2, $3, $4, $5, $6, $7}'
# 0 : menuentry 'Ubuntu, with Linux 6.8.0-41-generic' --class ubuntu
# 1 : menuentry 'Ubuntu, with Linux 6.8.0-41-generic (recovery mode)'
# 2 : menuentry 'Ubuntu, with Linux 6.8.0-31-generic' --class ubuntu
# 3 : menuentry 'Ubuntu, with Linux 6.8.0-31-generic (recovery mode)'
```

Change default:
```bash
sudo nano /etc/default/grub

# change N with correct number
# GRUB_DEFAULT="1>N"

sudo update-grub
```

reboot

## Disable kaslr on VM

on VM:
```bash
sudo nano /etc/default/grub

# add nokaslr to cmdline
# GRUB_CMDLINE_LINUX_DEFAULT="quiet splash nokaslr"

sudo update-grub
```
then reboot

## Open gdb port on qemu

```bash
virsh edit VM_NAME
# choose 1 for nano

# edit line of domain to this: <domain xmlns:qemu="http://libvirt.org/schemas/domain/qemu/1.0" type="kvm">
# add this immediatly under:

# <qemu:commandline>
#      <qemu:arg value='-s'/>
# </qemu:commandline>
```

if it works, we have an open "1234" port on host:
```bash
sudo netstat -tulpn | grep qemu
# tcp        0      0 127.0.0.1:5900          0.0.0.0:*               LISTEN      1147199/qemu-system
# tcp        0      0 0.0.0.0:1234            0.0.0.0:*               LISTEN      1147199/qemu-system
# tcp6       0      0 :::1234                 :::*                    LISTEN      1147199/qemu-system
```

## Add lx- scripts to GDB

Compile
```bash
make menu_config # exit immediatly
make scripts_gdb
```

Replace paths in qemu_kernel.gdb

## 

## Links:

Qemu - Virt manager: https://forum.manjaro.org/t/how-to-setting-up-qemu-kvm-with-virt-manager/127431

Compile ubuntu kernel: https://wiki.ubuntu.com/Kernel/BuildYourOwnKernel

GDB debugging: https://wiki.st.com/stm32mpu/wiki/Debugging_the_Linux_kernel_using_the_GDB
