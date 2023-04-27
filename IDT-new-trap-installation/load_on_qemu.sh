#!/bin/sh

qemu_ip=192.168.122.91
qemu_user=user
qemu_folder=/home/user/Desktop
folder_path=/home/user/Desktop/IDT-new-trap-installation

ssh $qemu_user@$qemu_ip "rm -rf $qemu_folder/IDT-new-trap-installation"

scp -i ~/.ssh/id_rsa -r $folder_path $qemu_user@$qemu_ip:$qemu_folder
