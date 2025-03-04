#ifndef CHAPTER2_CASEY_SIM86
#define CHAPTER2_CASEY_SIM86

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef int32_t b32;

#define ARRAY_COUNT(Array) (sizeof(Array) / sizeof((Array)[0]))

typedef enum {
    Op_None,
#define INST(Mnemonic, ...) Op_##Mnemonic,
#define INSTALT(...)
#include "sim86_instruction_table.inl"

    Op_Count,
} OperationType;

typedef enum {
    Inst_Lock = (1 << 0),
    Inst_Rep = (1 << 1),
    Inst_Segment = (1 << 2),
    Inst_Wide = (1 << 3),
} InstructionFlag;

typedef enum {
    Register_none,

    Register_a,
    Register_b,
    Register_c,
    Register_d,
    Register_sp,
    Register_bp,
    Register_si,
    Register_di,
    Register_es,
    Register_ss,
    Register_ds,
    Register_ip,
    Register_flags,
    
    Register_count,
} RegisterIndex;

typedef enum {
    EffectiveAddress_direct,

    EffectiveAddress_bx_si,
    EffectiveAddress_bx_di,
    EffectiveAddress_bp_si,
    EffectiveAddress_bp_di,
    EffectiveAddress_si,
    EffectiveAddress_di,
    EffectiveAddress_bp,
    EffectiveAddress_bx,

    EffectiveAddress_count,
} EffectiveAddressBase;

typedef struct {
    RegisterIndex segment;
    EffectiveAddressBase base;
    i32 displacement;
} EffectiveAddressExpression;

typedef struct {
    RegisterIndex index;
    u8 offset;
    u8 count;
} RegisterAccess;

typedef enum {
    Operand_None,
    Operand_Register,
    Operand_Memory,
    Operand_Immediate,
    Operand_RelativeImmediate,
} OperandType;

typedef struct {
    OperandType type;
    union 
    {
        EffectiveAddressExpression address;
        RegisterAccess _register;
        u32 immediate_u32;
        i32 immediate_i32;
    };
} InstructionOperand;

typedef struct {
    u32 address;
    u32 size;
    OperationType op;
    u32 flags;
    InstructionOperand operands[2];
} Instruction;

#endif /* CHAPTER2_CASEY_SIM86 */
