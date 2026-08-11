#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>

/*
    31 FE FF | AF | 21 FF 9F 32 CB 7C 20 FB 21 26 FF 0E 11 3E 80 32 E2 0C 3E F3 E2 32 3E 77 77 3E FC E0 47 11 04 01 21 10 80 1A CD 95 00 CD 96 00 13 7B FE 34 20 F3 
    11 D8 00 06 08 1A 13 22 23 05 20 F9 3E 19 EA 10 99 21 2F 99 0E 0C 3D 28 08 32 0D 20 F9 2E 0F 18 F3 67 3E 64 57 E0 42 3E 91 E0 40 04 1E 02 0E 0C F0 44 FE 90 
    20 FA 0D 20 F7 1D 20 F2 0E 13 24 7C 1E 83 FE 62 28 06 1E C1 FE 64 20 06 7B E2 0C 3E 87 E2 F0 42 90 E0 42 15 20 D2 05 20 4F 16 20 18 CB 4F 06 04 C5 CB 11 17 
    C1 CB 11 17 05 20 F5 22 23 22 23 C9 CE ED 66 66 CC 0D 00 0B 03 73 00 83 00 0C 00 0D 00 08 11 1F 88 89 00 0E DC CC 6E E6 DD DD D9 99 BB BB 67 63 6E 0E EC CC 
    DD DC 99 9F BB B9 33 3E 3C 42 B9 A5 B9 A5 42 3C 21 04 01 11 A8 00 1A 13 BE 20 FE 23 7D FE 34 20 F5 06 19 78 86 23 05 20 FB 86 20 FE 3E 01 E0 50 

                                                                    --BOOTROM--
*/


typedef union {
    uint16_t value;
    struct {
        uint8_t lo;
        uint8_t hi;
    };
} Reg16;

typedef struct {

    Reg16 af;
    Reg16 bc;
    Reg16 de;
    Reg16 hl;

    uint8_t *Rom; // My ROM buffer uint8_t 
    size_t rom_size;

    uint16_t sp;
    uint16_t pc;
} Registers;

uint8_t read_byte(Registers *cpu, uint16_t address)
{
    return cpu->Rom[address];
}

uint16_t read_word(Registers *cpu, uint16_t address)
{
    uint8_t lo = read_byte(cpu, address);
    uint8_t hi = read_byte(cpu, address + 1);

    return (uint16_t)(lo | (hi << 8));
}


void opcodes(Registers *cpu) {

    // (u16) or (u8) = pointer deference

    // op dest,src; ex -> ld AF, 10; AF = 10; 

    uint8_t read_Opcode = read_byte(cpu ,cpu->pc);
    
    switch (read_Opcode) {

        case 0x0: // NOP;
            cpu->pc += 1;
            break;

        case 0x21: // LD HL,u16
            cpu->hl = read_word();
        
        case 0xAF: // XOR A, A;
            cpu->af.hi = cpu->af.hi ^ cpu->af.hi;
            cpu->pc++;
            break;

        case 0xFA: //LD A, (u16);
            cpu->af.lo = read_word(cpu, cpu->pc + 1);
            cpu->pc += 3;
            break;


        case 0x31: //LD SP, u16;
            cpu->sp = read_word(cpu, cpu->pc + 1);
            cpu->pc += 3;
            break;
        
        default:
            printf("-Opcodes not found : %02X\n",read_Opcode);
            break;
        }
}

bool Cartridge_load(Registers *cart, const char *path)
{
    FILE *file = fopen(path, "rb");
    
    if (file == NULL) {
        perror("Error with fopen ");
        return false;
    } 
    else {
    fseek(file, 0, SEEK_END); 
    cart->rom_size = ftell(file);

    rewind(file);
    cart->Rom = malloc(cart->rom_size);

    fread(cart->Rom, 1, cart->rom_size , file);
    fclose(file);

    return true;
    }
}

int main(void){

    Registers *ROMA;
    Cartridge_load(ROMA,"/home/juan/Bureau/eco-system/Code/C,C++/Game_Boy_Emu/ROMS/[BIOS] Nintendo Game Boy Boot ROM (World) (Rev 1).gb");
    ROMA->pc = 3;
    opcodes(ROMA);

    printf("SP = %i\n",ROMA->sp);
    printf("PC = %i\n",ROMA->pc);
    printf("A = %i\n",ROMA->af.hi);

    return 0;
}
