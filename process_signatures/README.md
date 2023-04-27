# Evanding probes

What we want: calling kernel API and evade probes

## How does a function start

90% of Kernel functions start in:

```
   0x065:	nop    DWORD PTR [rax+rax*1+0x0]
   0x06a:	push   rbp
   0x06b:	mov    rbp,rsp
   0x06e:	sub    rsp,0x50
   0x072:	mov    QWORD PTR [rsp+0x28],rsi
```

## How does it look after the probe

- Example 2: 
```
   0x065:	call   0x3f4ed1d0
   0x06a:	push   rbp
   0x06b:	mov    rbp,rsp
   0x06e:	sub    rsp,0x50
   0x072:	mov    QWORD PTR [rsp+0x28],rsi
```

## How do we evade

jump to start+5

## How it is implemented

TODO: add description

Does it work? Yes

```
[  122.872605] [KPROBE] A printk call is starting...
[  122.872605] This is a normal printk call                       <---- Probe is activated on normal call
[  122.872605] [KPROBE] A printk call is ending...
[  122.882607] This is smart printk call                          <---- Probe is not activated on smart call
```