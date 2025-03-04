#include "sim86_decode.h"

static InstructionFormat instruction_formats[] = {
#include "sim86_instruction_table.inl"
};

DisAsmContext default_dis_asm_context(void)
{
    DisAsmContext result = {};
    result.default_segment = Register_ds;
    return result;
}

static InstructionOperand get_reg_operand(u32 intel_reg_index, b32 wide)
{
    RegisterAccess reg_table[][2] =
        {
            {{Register_a, 0, 1}, {Register_a, 0, 2}},
            {{Register_c, 0, 1}, {Register_c, 0, 2}},
            {{Register_d, 0, 1}, {Register_d, 0, 2}},
            {{Register_b, 0, 1}, {Register_b, 0, 2}},
            {{Register_a, 1, 1}, {Register_sp, 0, 2}},
            {{Register_c, 1, 1}, {Register_bp, 0, 2}},
            {{Register_d, 1, 1}, {Register_si, 0, 2}},
            {{Register_b, 1, 1}, {Register_di, 0, 2}},
        };

    InstructionOperand result = {};
    result.type = Operand_Register;
    result._register = reg_table[intel_reg_index & 0x7][(wide != 0)];

    return result;
}

static u32 parse_data_value(Memory *memory, SegmentedAccess *access, b32 exists, b32 wide, b32 sign_extended)
{
    u32 result = 0;
    if (exists)
    {
        if (wide)
        {
            u8 d0 = read_memory(memory, get_absolute_address_of_struct(*access, 0));
            u8 d1 = read_memory(memory, get_absolute_address_of_struct(*access, 1));
            result = (d1 << 8) | d0;
            access->segment_offset += 2;
        }
        else
        {
            result = read_memory(memory, get_absolute_address_of_struct(*access, 0));
            if (sign_extended)
            {
                result = (i32) * (i8 *)&result;
            }
            access->segment_offset += 1;
        }
    }
    return result;
}

static Instruction try_decode(DisAsmContext *context, InstructionFormat *inst, Memory *memory, SegmentedAccess at)
{
    Instruction dest = {};
    u32 has[Bits_Count] = {};
    u32 bits[Bits_Count] = {};
    b32 valid = true;

    u32 starting_address = get_absolute_address_of_struct(at, 0);

    u8 bits_pending_count = 0;
    u8 bits_pending = 0;

    for (u32 bits_index = 0; valid && (bits_index < ARRAY_COUNT(inst->bits)); ++bits_index)
    {
        InstructionBits test_bits = inst->bits[bits_index];
        if ((test_bits.usage == Bits_Literal) && (test_bits.bit_count == 0))
        {
            break;
        }
        u32 read_bits = test_bits.value;
        if (test_bits.bit_count != 0)
        {
            if (bits_pending_count == 0)
            {
                bits_pending_count = 8;
                bits_pending = read_memory(memory, get_absolute_address_of_struct(at, 0));
                ++at.segment_offset;
            }
            assert(test_bits.bit_count <= bits_pending_count);

            bits_pending_count -= test_bits.bit_count;
            read_bits = bits_pending;
            read_bits >>= bits_pending_count;
            read_bits &= ~(0xff << test_bits.bit_count);
        }

        if (test_bits.usage == Bits_Literal)
        {
            valid = valid && (read_bits == test_bits.value);
        }
        else
        {
            bits[test_bits.usage] |= (read_bits << test_bits.shift);
            has[test_bits.usage] = true;
        }
    }

    if (valid)
    {
        u32 mod = bits[Bits_MOD];
        u32 rm = bits[Bits_RM];
        u32 w = bits[Bits_W];
        b32 s = bits[Bits_S];
        b32 d = bits[Bits_D];

        b32 has_direct_address = ((mod == 0b00) && (rm == 0b110));
        b32 has_displacement = ((bits[Bits_HasDisp]) || (mod == 0b10) || has_direct_address);
        b32 displacement_is_w = ((bits[Bits_DispAlwaysW]) || (mod == 0b10) || has_direct_address);
        b32 data_is_w = ((bits[Bits_WMakesDataW]) && !s && w);

        bits[Bits_Disp] |= parse_data_value(memory, &at, has_displacement, displacement_is_w, (!displacement_is_w));
        bits[Bits_Data] |= parse_data_value(memory, &at, bits[Bits_HasData], data_is_w, s);

        dest.op = inst->op;
        dest.flags = context->additional_flags;
        dest.address = starting_address;
        dest.size = get_absolute_address_of_struct(at, 0) - starting_address;
        if (w)
        {
            dest.flags |= Inst_Wide;
        }

        u32 disp = bits[Bits_Disp];
        i16 displacement = (i16)disp;

        InstructionOperand *reg_operand = &dest.operands[d ? 0 : 1];
        InstructionOperand *mod_operand = &dest.operands[d ? 1 : 0];

        if (has[Bits_SR])
        {
            reg_operand->type = Operand_Register;
            reg_operand->_register.index = (RegisterIndex)(Register_es + (bits[Bits_SR] & 0x3));
            reg_operand->_register.count = 2;
        }
        if (has[Bits_REG])
        {
            *reg_operand = get_reg_operand(bits[Bits_REG], w);
        }

        if (has[Bits_MOD])
        {
            if (mod == 0b11)
            {
                *mod_operand = get_reg_operand(rm, w || (bits[Bits_RMRegAlwaysW]));
            }
            else
            {
                mod_operand->type = Operand_Memory;
                mod_operand->address.segment = context->default_segment;
                mod_operand->address.displacement = displacement;

                if ((mod == 0b00) && (rm == 0b110))
                {
                    mod_operand->address.base = EffectiveAddress_direct;
                }
                else
                {
                    mod_operand->address.base = (EffectiveAddressBase)(1 + rm);
                }
            }
        }

        InstructionOperand *last_operand = &dest.operands[0];
        if (last_operand->type)
        {
            last_operand = &dest.operands[1];
        }
        if (bits[Bits_RelJMPDisp])
        {
            last_operand->type = Operand_RelativeImmediate;
            last_operand->immediate_i32 = displacement + dest.size;
        }
        if (bits[Bits_HasData])
        {
            last_operand->type = Operand_Immediate;
            last_operand->immediate_u32 = bits[Bits_Data];
        }

        if (has[Bits_V])
        {
            if (bits[Bits_V])
            {
                last_operand->type = Operand_Register;
                last_operand->_register.index = Register_c;
                last_operand->_register.offset = 0;
                last_operand->_register.count = 1;
            }
            else
            {
                last_operand->type = Operand_Immediate;
                last_operand->immediate_i32 = 1;
            }
        }
    }
    return dest;
}

Instruction decode_instruction(DisAsmContext *context, Memory *memory, SegmentedAccess *at)
{
    Instruction result = {};
    for (u32 index = 0; index < ARRAY_COUNT(instruction_formats); ++index)
    {
        InstructionFormat inst = instruction_formats[index];
        result = try_decode(context, &inst, memory, *at);
        if (result.op)
        {
            at->segment_offset += result.size;
            break;
        }
    }
    return result;
}

void update_context(DisAsmContext *context, Instruction instruction)
{
    if (instruction.op == Op_lock)
    {
        context->additional_flags |= Inst_Lock;
    }
    else if (instruction.op == Op_rep)
    {
        context->additional_flags |= Inst_Rep;
    }
    else if (instruction.op == Op_segment)
    {
        context->additional_flags |= Inst_Segment;
        context->default_segment = instruction.operands[1]._register.index;
    }
    else
    {
        context->additional_flags = 0;
        context->default_segment = Register_ds;
    }
}