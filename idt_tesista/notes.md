sysvec address:
```
 0xffffffff81caa000 <sysvec_spurious_apic_interrupt>: push   r12
```

memory address;
```
gef➤  x/i $r12
0xffffffff81e00e21 <asm_sysvec_spurious_apic_interrupt+17>:  call   0xffffffff81caa000 <sysvec_spurious_apic_interrupt>
```

rdx =  0x00000081caa000 