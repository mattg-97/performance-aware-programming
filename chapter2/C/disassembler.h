#ifndef CHAPTER2_C_DISASSEMBLER
#define CHAPTER2_C_DISASSEMBLER

#include "sim86.h"

typedef enum
{
    NONE,
    _8_BIT,
    _16_BIT,
} Displacement;

void disassemble_instructions(u8* instructions, u64 length);

#endif /* CHAPTER2_C_DISASSEMBLER */
