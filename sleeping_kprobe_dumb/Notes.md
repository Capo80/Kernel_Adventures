# ELF loading

main function:
```
https://elixir.bootlin.com/linux/v6.0/source/fs/binfmt_elf.c#L824
```

loops that loads each section
```
https://elixir.bootlin.com/linux/v6.0/source/fs/binfmt_elf.c#L1035
```

section loader function
```
https://elixir.bootlin.com/linux/v6.0/source/fs/binfmt_elf.c#L365

```

ELF section is not materialized:
fault happens here 
```
https://elixir.bootlin.com/linux/v6.0/source/mm/util.c#L556
```
but populate is false beacuse this:
```
https://elixir.bootlin.com/linux/v6.0/source/mm/mmap.c#L1541
```

actual mmap happens with the driver of the file:
```
https://elixir.bootlin.com/linux/v6.0/source/mm/mmap.c#L1752
```

