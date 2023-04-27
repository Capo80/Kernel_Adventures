# Kernel API analysis

kprobes are usually implemented with a ```int3``` instruction, but they can be optimized with a ```call``` or ```jmp``` instruction for 5x permormance boost.

kprobe optimization is activated with the CONFIG_OPTPROBES kernel compilation flaf, enable by default on x86_64

Not all functions are compatible with the ```call``` instruction. in paticular:

```

Safety Check¶

Before optimizing a probe, Kprobes performs the following safety checks:

   - Kprobes verifies that the region that will be replaced by the jump instruction (the “optimized region”) lies entirely within one function. (A jump instruction is multiple bytes, and so may overlay multiple instructions.)

   - Kprobes analyzes the entire function and verifies that there is no jump into the optimized region. Specifically:

      - the function contains no indirect jump;

      - the function contains no instruction that causes an exception (since the fixup code triggered by the exception could jump back into the optimized region – Kprobes checks the exception tables to verify this);

      - there is no near jump to the optimized region (other than to the first byte).

      - For each instruction in the optimized region, Kprobes verifies that the instruction can be executed out of line.


```