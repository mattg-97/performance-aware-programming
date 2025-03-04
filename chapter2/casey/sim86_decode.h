#ifndef CHAPTER2_CASEY_SIM86_DECODE
#define CHAPTER2_CASEY_SIM86_DECODE

#include "sim86.h"
#include "sim86_memory.h"

typedef enum {
    Bits_Literal,
    Bits_MOD,
    Bits_REG,
    Bits_RM,
    Bits_SR,
    Bits_Disp,
    Bits_Data,

    Bits_HasDisp,
    Bits_DispAlwaysW,
    Bits_HasData,
    Bits_WMakesDataW,
    Bits_RMRegAlwaysW,
    Bits_RelJMPDisp,
    Bits_D,
    Bits_S,
    Bits_W,
    Bits_V,
    Bits_Z,
    
    Bits_Count,
} InstructionBitsUsage;

typedef struct {
    InstructionBitsUsage usage;
    u8 bit_count;
    u8 shift;
    u8 value;
} InstructionBits;

typedef struct {
    OperationType op;
    InstructionBits bits[16];
} InstructionFormat;

typedef struct {
    RegisterIndex default_segment;
    u32 additional_flags;
} DisAsmContext;

DisAsmContext default_dis_asm_context(void);
Instruction decode_instruction(DisAsmContext *context, Memory *memory, SegmentedAccess *at);
void update_context(DisAsmContext *context, Instruction instruction);
#endif /* CHAPTER2_CASEY_SIM86_DECODE */
