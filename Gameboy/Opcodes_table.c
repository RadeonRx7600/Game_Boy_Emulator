/* I used Ai generation for this file "Opcodes_table.c"
   I ask Claude to do it you can see the prompt in the
   file "Claude_prompt_op.c".
*/

#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>

static void flags_add8(Registers *cpu, uint8_t a, uint8_t val) {
    uint16_t result = (uint16_t)a + (uint16_t)val;
    cpu->zf = ((uint8_t)result == 0);
    cpu->nf = false;
    cpu->hf = (((a & 0xF) + (val & 0xF)) > 0xF);
    cpu->cf = (result > 0xFF);
}

static void flags_adc8(Registers *cpu, uint8_t a, uint8_t val, uint8_t carry) {
    uint16_t result = (uint16_t)a + (uint16_t)val + (uint16_t)carry;
    cpu->zf = ((uint8_t)result == 0);
    cpu->nf = false;
    cpu->hf = (((a & 0xF) + (val & 0xF) + carry) > 0xF);
    cpu->cf = (result > 0xFF);
}

static void flags_sub8(Registers *cpu, uint8_t a, uint8_t val) {
    uint8_t result = a - val;
    cpu->zf = (result == 0);
    cpu->nf = true;
    cpu->hf = ((a & 0xF) < (val & 0xF));
    cpu->cf = (a < val);
}

static void flags_sbc8(Registers *cpu, uint8_t a, uint8_t val, uint8_t carry) {
    int16_t result = (int16_t)a - (int16_t)val - (int16_t)carry;
    cpu->zf = ((uint8_t)result == 0);
    cpu->nf = true;
    cpu->hf = (((int16_t)(a & 0xF) - (int16_t)(val & 0xF) - (int16_t)carry) < 0);
    cpu->cf = (result < 0);
}

static void flags_and8(Registers *cpu, uint8_t result) {
    cpu->zf = (result == 0);
    cpu->nf = false;
    cpu->hf = true;
    cpu->cf = false;
}

static void flags_or8(Registers *cpu, uint8_t result) {
    cpu->zf = (result == 0);
    cpu->nf = false;
    cpu->hf = false;
    cpu->cf = false;
}

static void flags_xor8(Registers *cpu, uint8_t result) {
    cpu->zf = (result == 0);
    cpu->nf = false;
    cpu->hf = false;
    cpu->cf = false;
}

static void flags_cp8(Registers *cpu, uint8_t a, uint8_t val) {
    flags_sub8(cpu, a, val);
}

static void flags_inc8(Registers *cpu, uint8_t result) {
    cpu->zf = (result == 0);
    cpu->nf = false;
    cpu->hf = ((result & 0xF) == 0x0);
}

static void flags_dec8(Registers *cpu, uint8_t result) {
    cpu->zf = (result == 0);
    cpu->nf = true;
    cpu->hf = ((result & 0xF) == 0xF);
}

static uint16_t add_hl16(Registers *cpu, uint16_t hl, uint16_t val) {
    uint32_t result = (uint32_t)hl + (uint32_t)val;
    cpu->nf = false;
    cpu->hf = (((hl & 0xFFF) + (val & 0xFFF)) > 0xFFF);
    cpu->cf = (result > 0xFFFF);
    return (uint16_t)result;
}

static void flags_sp_r8(Registers *cpu, uint16_t sp, int8_t r8) {
    uint8_t val = (uint8_t)r8;
    cpu->zf = false;
    cpu->nf = false;
    cpu->hf = (((sp & 0xF) + (val & 0xF)) > 0xF);
    cpu->cf = (((sp & 0xFF) + val) > 0xFF);
}

static void push16(RAM *ram, Registers *cpu, uint16_t value) {
    cpu->sp--;
    write_memory8(ram, (uint8_t)(value >> 8), cpu->sp);
    cpu->sp--;
    write_memory8(ram, (uint8_t)(value & 0xFF), cpu->sp);
}

static uint16_t pop16(RAM *ram, Registers *cpu) {
    uint8_t lo = read_memory8(ram, cpu->sp);
    cpu->sp++;
    uint8_t hi = read_memory8(ram, cpu->sp);
    cpu->sp++;
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
