#!/bin/sh

qemu_ip=192.168.122.91
qemu_user=user
qemu_folder=/home/user/Desktop
folder_path=/home/user/Desktop/execute_user_memory

ssh $qemu_user@$qemu_ip "rm -rf $qemu_folder/execute_user_memory"

scp -i ~/.ssh/id_rsa -r $folder_path $qemu_user@$qemu_ip:$qemu_folder
