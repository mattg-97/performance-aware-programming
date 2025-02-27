# Instruction decoding on the 8086
- 'mov' is technically a copy, it copies data from a to b
- lets suppose we want to take the data from bx and 'move' it to ax
```arm
#ax is the destination register
#bx is the source register
mov ax, bx
```
- this instruction in 8086 is encoded as 2 bytes
- [100010dw][mod(2),reg(3),rm(3)]
- mod: register to memory or register to register mode, 11 would be register to register.
- reg: the register to move the data to.
- rm: in our specific case this is the source register.
- d: if d bit is 0 it means reg is NOT the destination
- w: is this 16 bits or not?
## Bit targeting
```arm
#this copies the ENTIRE register
mov ax, bx
#this copies the low bits 
mov al, bl
#this copies the high bits 
mov ah, bh
```