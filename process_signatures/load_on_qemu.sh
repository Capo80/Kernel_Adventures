#!/bin/sh

qemu_ip=192.168.122.196
qemu_user=user
qemu_folder=/home/user/Desktop
folder_path=/home/user/Desktop/kernel_adventures/process_signatures

ssh $qemu_user@$qemu_ip "rm -rf $qemu_folder/process_signatures"

scp -i ~/.ssh/id_rsa -r $folder_path $qemu_user@$qemu_ip:$qemu_folder
