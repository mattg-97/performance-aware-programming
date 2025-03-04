#ifndef CHAPTER2_CASEY_SIM86_MEMORY
#define CHAPTER2_CASEY_SIM86_MEMORY

#include "sim86.h"

typedef struct {
    u8 bytes[1024*1024];
}Memory ;

#define MEMORY_ACCESS_MASK 0xfffff
//Memorystatic_assert((ARRAY_COUNT(Memory.bytes) - 1) == MEMORY_ACCESS_MASK, "Memory size doesnt match access mask");

typedef struct {
    u16 segment_base;
    u16 segment_offset;
} SegmentedAccess;

u32 get_absolute_address_of_manual(u16 segment_base, u16 segment_offset, u16 additional_offset);
u32 get_absolute_address_of_struct(SegmentedAccess access, u16 additional_offset);

u8 read_memory(Memory *memory, u32 absolute_address);

u32 load_memory_from_file(char *file_name, Memory *memory, u32 at_offset);

#endif /* CHAPTER2_CASEY_SIM86_MEMORY */
