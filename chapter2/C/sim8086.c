#include <stdlib.h>
#include <stdio.h>

#include "disassembler.h"
#include "sim86.h"

typedef struct {
    u8* data;
    u64 length;
} FileData;

static FileData read_file(const char *fileName)
{
    FILE *file = fopen(fileName, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", fileName);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0L, SEEK_END);
    u64 file_size = (u64)ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read: %s\n", fileName);
        exit(EXIT_FAILURE);
    }
    u64 bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size)
    {
        fprintf(stderr, "Could not read file: %s\n", fileName);
        exit(EXIT_FAILURE);
    }
    fclose(file);
    return (FileData){
        .data = (u8 *)buffer,
        .length = bytes_read,
    };
}


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "You must provide a single argument to this executable.\n");
        exit(EXIT_FAILURE);
    }
    FileData file_data = read_file(argv[1]);

    disassemble_instructions(file_data.data, file_data.length);

    free(file_data.data);
    return 0;
}