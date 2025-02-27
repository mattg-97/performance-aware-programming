#include <stdlib.h>
#include <stdio.h>

typedef unsigned long long u64;
typedef unsigned char u8;

static u8 *read_file(const char *fileName)
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
    buffer[bytes_read] = '\0';
    fclose(file);
    return (u8 *)buffer;
}

void disassemble_instructions(u8 *instructions)
{
    while (*instructions != '\0')
    {
        // shift 2 bits to the right to get first 6 bits
        // then mask it using AND, this will leave us with the opcode
        u8 opcode = (*instructions >> 2) & 0b00111111;
        switch (opcode)
        {
        case 0b00100010:
        {
            printf("mov ");
            u8 d = (*instructions & 0b00000010) >> 1;
            u8 w = (*instructions & 0b00000001);
            instructions += 1;
            u8 mod = (*instructions >> 6) & 0b00000011;
            u8 reg = (*instructions >> 3) & 0b00000111;
            u8 rm = *instructions & 0b00000111;
            if (mod == 0b11)
            {
                switch (rm)
                {
                case 0b000:
                {
                    w == 0 ? printf("AL") : printf("AX");
                    break;
                }
                case 0b001:
                {
                    w == 0 ? printf("CL") : printf("CX");
                    break;
                }
                case 0b010:
                {
                    w == 0 ? printf("DL") : printf("DX");
                    break;
                }
                case 0b011:
                {
                    w == 0 ? printf("BL") : printf("BX");
                    break;
                }
                case 0b100:
                {
                    w == 0 ? printf("AH") : printf("SP");
                    break;
                }
                case 0b101:
                {
                    w == 0 ? printf("CH") : printf("BP");
                    break;
                }
                case 0b110:
                {
                    w == 0 ? printf("DH") : printf("SI");
                    break;
                }
                case 0b111:
                {
                    w == 0 ? printf("BH") : printf("DI");
                    break;
                }
                }
            }
            printf(", ");
            switch (reg)
            {
            case 0b000:
            {
                w == 0 ? printf("AL") : printf("AX");
                break;
            }
            case 0b001:
            {
                w == 0 ? printf("CL") : printf("CX");
                break;
            }
            case 0b010:
            {
                w == 0 ? printf("DL") : printf("DX");
                break;
            }
            case 0b011:
            {
                w == 0 ? printf("BL") : printf("BX");
                break;
            }
            case 0b100:
            {
                w == 0 ? printf("AH") : printf("SP");
                break;
            }
            case 0b101:
            {
                w == 0 ? printf("CH") : printf("BP");
                break;
            }
            case 0b110:
            {
                w == 0 ? printf("DH") : printf("SI");
                break;
            }
            case 0b111:
            {
                w == 0 ? printf("BH") : printf("DI");
                break;
            }
            }
            printf("\n");
            break;
        }
        }
        instructions += 1;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "You must provide a single argument to this executable.\n");
        exit(EXIT_FAILURE);
    }
    u8 *file_bytes = read_file(argv[1]);

    disassemble_instructions(file_bytes);

    free(file_bytes);
    return 0;
}