#include "disassembler.h"

void print_default_reg_table(u8 w, u8 reg)
{
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
}

static u8 *print_mod_00(u8 *instructions, u8 rm, bool direct_address, Displacement displacement)
{
    switch (rm)
    {
    case 0b000:
        printf("[BX + SI");
        break;
    case 0b001:
        printf("[BX + DI");
        break;
    case 0b010:
        printf("[BP + SI");
        break;
    case 0b011:
        printf("[BP + DI");
        break;
    case 0b100:
        printf("[SI");
        break;
    case 0b101:
        printf("[DI");
        break;
    case 0b110:
    {
        direct_address ? printf("DIRECT ADDRESS") : printf("[BP");
        break;
    }
    case 0b111:
        printf("[BX");
        break;
    }
    switch (displacement)
    {
    case NONE:
    {
        printf("]");
        break;
    }
    case _8_BIT:
    {
        instructions += 1;
        if (*instructions == 0) {
            printf("]");
            break;
        }
        printf(" + %d]", (int8_t)*instructions);
        break;
    }
    case _16_BIT:
    {
        instructions += 1;
        u8 low_byte = *instructions;
        instructions += 1;
        u8 high_byte = *instructions;
        u16 data = low_byte | (high_byte << 8);
        printf(" + %d]", (int16_t)data);
        break;
    }
    }
    return instructions;
}

u8 *disassemble_reg_reg_mov(u8 *instructions)
{
    printf("mov ");
    u8 d = (*instructions & 2) >> 1;   // d = second to last bit
    u8 w = (*instructions & 1);        // w = last bit
    instructions += 1;                 // go to next byte
    u8 mod = (*instructions >> 6) & 3; // first 2 bits
    u8 reg = (*instructions >> 3) & 7; // middle 3 bits
    u8 rm = *instructions & 7;         // last 3 bits
    switch (mod)
    {
    case 0b11: // register to register MOV
    {
        instructions += 1;
        print_default_reg_table(w, rm);
        printf(", ");
        print_default_reg_table(w, reg);
        break;
    }
    case 0b00:
    {
        if (d == 1)
        {
            // Format: mov reg, [memory]
            print_default_reg_table(w, reg);
            printf(", ");
            if (rm == 0b110)
            {
                instructions = print_mod_00(instructions, rm, true, _16_BIT);
            }
            else
            {
                instructions = print_mod_00(instructions, rm, true, NONE);
            }
        }
        else
        {
            // Format: mov [memory], reg
            if (rm == 0b110)
            {
                instructions = print_mod_00(instructions, rm, true, _16_BIT);
            }
            else
            {
                instructions = print_mod_00(instructions, rm, true, NONE);
            }
            printf(", ");
            print_default_reg_table(w, reg);
        }
        break;
    }
    case 0b01:
    {
        if (d == 1)
        {
            // Format: mov reg, [memory]
            print_default_reg_table(w, reg);
            printf(", ");
            instructions = print_mod_00(instructions, rm, false, _8_BIT);
        }
        else
        {
            // Format: mov [memory], reg
            instructions = print_mod_00(instructions, rm, false, _8_BIT);
            printf(", ");
            print_default_reg_table(w, reg);
        }
        break;
    }
    case 0b10:
    {
        if (d == 1)
        {
            // Format: mov reg, [memory]
            print_default_reg_table(w, reg);
            printf(", ");
            instructions = print_mod_00(instructions, rm, false, _16_BIT);
        }
        else
        {
            // Format: mov [memory], reg
            instructions = print_mod_00(instructions, rm, false, _16_BIT);
            printf(", ");
            print_default_reg_table(w, reg);
        }
        instructions += 1;
        break;
    }
    }
    printf("\n");
    return instructions;
}

u8 *disassemble_imm_reg_mem_mov(u8 *instructions)
{
    printf("mov ");
    u8 w = (*instructions & 1); // w = last bit
    instructions += 1;
    u8 mod = (*instructions >> 6) & 3; // first 2 bits
    u8 rm = *instructions & 7;         // last 3 bits
    switch (mod)
    {
    case 0b00:
    {
        if (rm == 0b110)
        {
            instructions = print_mod_00(instructions, rm, true, _16_BIT);
        }
        else
        {
            instructions = print_mod_00(instructions, rm, true, NONE);
        }
        break;
    }
    case 0b01:
    {
        instructions = print_mod_00(instructions, rm, false, _8_BIT);
        printf(", ");
        print_default_reg_table(w, rm);
        break;
    }
    case 0b10:
    {
        instructions = print_mod_00(instructions, rm, false, _16_BIT);
        printf(", ");
        print_default_reg_table(w, rm);
        instructions += 1;
        break;
    }
    }
    printf("\n");
    return instructions + 1;
}

u8 *disassemble_imm_reg_mov(u8 *instructions)
{
    printf("mov ");
    u8 w = (*instructions >> 3) & 1;
    u8 reg = (*instructions & 0b00000111);
    print_default_reg_table(w, reg);
    instructions += 1;
    printf(", ");
    if (w == 1)
    {
        u8 low_byte = *instructions;
        instructions += 1;
        u8 high_byte = *instructions;
        u16 data = low_byte | (high_byte << 8);
        printf("%d", (int16_t)data);
    }
    else
    {
        printf("%d", (int8_t)*instructions);
    }
    printf("\n");
    return instructions + 1;
}

void disassemble_instructions(u8 *instructions, u64 length)
{
    u8 *end = instructions + length;
    while (instructions < end)
    {
        // shift 2 bits to the right to get first 6 bits
        // then mask it using AND, this will leave us with the opcode
        u8 reg_reg_mov_opcode = (*instructions >> 2) & 0x3F;
        if (reg_reg_mov_opcode == 0b00100010)
        {
            instructions = disassemble_reg_reg_mov(instructions);
        }
        else if ((*instructions >> 4) == 0b1011)
        {
            instructions = disassemble_imm_reg_mov(instructions);
        }
        else if (((*instructions >> 1) & 0b01111111) == 0b1100011)
        {
            instructions = disassemble_imm_reg_mem_mov(instructions);
        }
        else
        {
            instructions += 1;
        }
    }
}