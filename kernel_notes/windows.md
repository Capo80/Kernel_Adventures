# Windows Kernel notes

## ASM inline

ASM inline by itself is not possible, we to create a separate ASM and then add the function as an extern variable in the c:

```
// in c
extern void HookTrap0E();
extern void Trap0E_Ori();

// in asm
.code
HookTrap0E PROC PUBLIC
       JMP OldTrap0E
HookTrap0E ENDP
```

Note: Remember to set ```Item Type``` to ```Microsoft Macro Assembler``` inside the .asm file ```Properties```->```General```
