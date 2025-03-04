#include "sim86_memory.h"

u32 get_absolute_address_of_manual(u16 segment_base, u16 segment_offset, u16 additional_offset)
{
    u32 result = (((u32)segment_base << 4) + (u32)(segment_offset + additional_offset)) & MEMORY_ACCESS_MASK;
    return result;
}
u32 get_absolute_address_of_struct(SegmentedAccess access, u16 additional_offset)
{
    u32 result = get_absolute_address_of_manual(access.segment_base, access.segment_offset, additional_offset);
    return result;
}

u8 read_memory(Memory *memory, u32 absolute_address)
{
    assert(absolute_address < ARRAY_COUNT(memory->bytes));
    u8 result = memory->bytes[absolute_address];
    return result;
}

u32 load_memory_from_file(char *file_name, Memory *memory, u32 at_offset)
{
    u32 result = 0;
    if (at_offset < ARRAY_COUNT(memory->bytes))
    {
        FILE *file = fopen(file_name, "rb");
        if (file == NULL)
        {
            fprintf(stderr, "Could not open file: %s\n", file_name);
            exit(EXIT_FAILURE);
        }
        result = fread(memory->bytes + at_offset, 1, ARRAY_COUNT(memory->bytes) - at_offset, file);
        fclose(file);
    }
    return result;
}