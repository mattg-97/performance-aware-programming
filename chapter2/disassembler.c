#include "disassembler.h"

void disassemble_mov(u8 *instructions)
{
    printf("mov ");
    u8 d = (*instructions & 2) >> 1;   // d = second to last bit
    u8 w = (*instructions & 1);        // w = last bit
    instructions += 1;                 // go to next byte
    u8 mod = (*instructions >> 6) & 3; // first 2 bits
    u8 reg = (*instructions >> 3) & 7; // middle 3 bits
    u8 rm = *instructions & 7;         // last 3 bits
    if (mod == 0b11)                   // register to register MOV
    {
        switch (rm)
        {
        case 0b000:
            w == 0 ? printf("AL") : printf("AX");
            break;
        case 0b001:
            w == 0 ? printf("CL") : printf("CX");
            break;
        case 0b010:
            w == 0 ? printf("DL") : printf("DX");
            break;
        case 0b011:
            w == 0 ? printf("BL") : printf("BX");
            break;
        case 0b100:
            w == 0 ? printf("AH") : printf("SP");
            break;
        case 0b101:
            w == 0 ? printf("CH") : printf("BP");
            break;
        case 0b110:
            w == 0 ? printf("DH") : printf("SI");
            break;
        case 0b111:
            w == 0 ? printf("BH") : printf("DI");
            break;
        }
    }
    printf(", ");
    switch (reg)
    {
    case 0b000:
        w == 0 ? printf("AL") : printf("AX");
        break;
    case 0b001:
        w == 0 ? printf("CL") : printf("CX");
        break;
    case 0b010:
        w == 0 ? printf("DL") : printf("DX");
        break;
    case 0b011:
        w == 0 ? printf("BL") : printf("BX");
        break;
    case 0b100:
        w == 0 ? printf("AH") : printf("SP");
        break;
    case 0b101:
        w == 0 ? printf("CH") : printf("BP");
        break;
    case 0b110:
        w == 0 ? printf("DH") : printf("SI");
        break;
    case 0b111:
        w == 0 ? printf("BH") : printf("DI");
        break;
    }
    printf("\n");
}

void disassemble_instructions(u8 *instructions)
{
    while (*instructions != '\0')
    {
        // shift 2 bits to the right to get first 6 bits
        // then mask it using AND, this will leave us with the opcode
        u8 opcode = (*instructions >> 2) & 0x3F;
        switch (opcode)
        {
        case 0b00100010:
            disassemble_mov(instructions);
        }
        instructions += 1;
    }
}