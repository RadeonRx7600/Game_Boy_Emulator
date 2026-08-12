#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>




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

    return 0;
}
