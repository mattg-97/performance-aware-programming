#include "sim86_text.h"

char const *OpcodeMnemonics[] =
    {
        "",
#define INST(Mnemonic, ...) #Mnemonic,
#define INSTALT(...)
#include "sim86_instruction_table.inl"
};

static char const *get_mnemonic(OperationType op)
{
    char const *result = OpcodeMnemonics[op];
    return result;
}

static char const *get_reg_name(RegisterAccess reg)
{
    char const *names[][3] = {
        {"", "", ""},
        {"al", "ah", "ax"},
        {"bl", "bh", "bx"},
        {"cl", "ch", "cx"},
        {"dl", "dh", "dx"},
        {"sp", "sp", "sp"},
        {"bp", "bp", "bp"},
        {"si", "si", "si"},
        {"di", "di", "di"},
        {"es", "es", "es"},
        {"cs", "cs", "cs"},
        {"ss", "ss", "ss"},
        {"ds", "ds", "ds"},
        {"ip", "ip", "ip"},
        {"flags", "flags", "flags"}};
    // static_assert(ARRAY_COUNT(names) == Register_count, "Text table mismatch for register_index");
    char const *result = names[reg.index][(reg.count == 2) ? 2 : reg.offset & 1];
    return result;
}

static char const *get_effective_address_expression(EffectiveAddressExpression address)
{
    char const *rmbase[] = {
        "",
        "bx+si",
        "bx+di",
        "bp+si",
        "bp+di",
        "si",
        "di",
        "bp",
        "bx",
    };
    static_assert(ARRAY_COUNT(rmbase) == EffectiveAddress_count, "text table mismatch for effective_base_address");
    char const *result = rmbase[address.base];
    return result;
}

b32 is_printable(Instruction instruction)
{
    b32 result = !((instruction.op == Op_lock) ||
                   (instruction.op == Op_rep) ||
                   (instruction.op == Op_segment));
    return result;
}

void print_instruction(Instruction instruction, FILE *dest)
{
    u32 flags = instruction.flags;
    u32 w = flags & Inst_Wide;
    if (flags & Inst_Lock)
    {
        if (instruction.op == Op_xchg)
        {
            InstructionOperand temp = instruction.operands[0];
            instruction.operands[0] = instruction.operands[1];
            instruction.operands[1] = temp;
        }
        fprintf(dest, "lock ");
    }

    char const *mnemonic_suffix = "";
    if (flags & Inst_Rep)
    {
        printf("rep ");
        mnemonic_suffix = w ? "w" : "b";
    }
    fprintf(dest, "%s%s", get_mnemonic(instruction.op), mnemonic_suffix);

    char const *separator = " ";
    for (u32 operand_index = 0; operand_index < ARRAY_COUNT(instruction.operands); ++operand_index)
    {
        InstructionOperand operand = instruction.operands[operand_index];
        if (operand.type != Operand_None)
        {
            fprintf(dest, "%s", separator);
            separator = ", ";

            switch (operand.type)
            {
            case Operand_None:
                break;
            case Operand_Register:
            {
                fprintf(dest, "%s", get_reg_name(operand._register));
                break;
            }
            case Operand_Memory:
            {
                EffectiveAddressExpression address = operand.address;
                if (instruction.operands[0].type != Operand_Register)
                {
                    fprintf(dest, "%s ", w ? "word" : "byte");
                }
                if (flags & Inst_Segment)
                {
                    printf("%s:", get_reg_name((RegisterAccess){.index = address.segment, .offset = 0, .count = 2}));
                }

                fprintf(dest, "[%s", get_effective_address_expression(address));
                if (address.displacement != 0)
                {
                    fprintf(dest, "%+d", address.displacement);
                }
                fprintf(dest, "]");
                break;
            }
            case Operand_Immediate:
            {
                fprintf(dest, "%d", operand.immediate_i32);
                break;
            }
            case Operand_RelativeImmediate:
            {
                fprintf(dest, "$%+d", operand.immediate_i32);
                break;
            }
            }
        }
    }
}