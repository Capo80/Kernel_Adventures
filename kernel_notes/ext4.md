# ext4

## Write flow

Block allocation call from write happens here:

https://elixir.bootlin.com/linux/latest/source/fs/ext4/file.c#L299

Block allocation function is here (allocates more block at the time?):

https://elixir.bootlin.com/linux/latest/source/fs/ext4/inode.c#L751

https://elixir.bootlin.com/linux/latest/source/fs/ext4/inode.c#L478