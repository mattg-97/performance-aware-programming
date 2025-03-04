#include "sim86.h"
#include "sim86_decode.h"
#include "sim86_memory.h"
#include "sim86_text.h"

b32 is_printable(Instruction instruction);

static void dis_asm_8086(Memory *memory, u32 dis_asm_byte_count, SegmentedAccess dis_asm_start)
{
    SegmentedAccess at = dis_asm_start;

    DisAsmContext context = default_dis_asm_context();

    u32 count = dis_asm_byte_count;
    while (count)
    {
        Instruction inst = decode_instruction(&context, memory, &at);
        if (inst.op)
        {
            if (count >= inst.size)
            {
                count -= inst.size;
            }
            else
            {
                fprintf(stderr, "ERROR: instruction extends outside disassembly region.\n");
                break;
            }
            update_context(&context, inst);
            if (is_printable(inst))
            {
                print_instruction(inst, stdout);
                printf("\n");
            }
        }
        else
        {
            fprintf(stderr, "ERROR: Unrecognised binary in instruciton stream\n");
            break;
        }
    }
}

int main(int argc, char **argv)
{
    Memory *memory = (Memory *)malloc(sizeof(Memory));
    if (argc > 1)
    {
        for (int arg_index = 1; arg_index < argc; ++arg_index)
        {
            char *file_name = argv[arg_index];
            u32 bytes_read = load_memory_from_file(file_name, memory, 0);
            printf("; %s disassembly:\n", file_name);
            printf("bits 16\n");
            dis_asm_8086(memory, bytes_read, (SegmentedAccess){});
        }
    }
    else
    {
        fprintf(stderr, "USAGE: %s [8086 machine code file] ...\n", argv[0]);
    }
    return 0;
}