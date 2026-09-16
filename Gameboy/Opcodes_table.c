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

uint8_t Execute_Opcode(Registers *cpu, Cartridge *cart ,RAM *ram) {

    static int index = 0;
    uint8_t read_Opcode = read_byte(cart ,cpu->pc);
    printf("opcode = 0x%02X",read_Opcode);
    printf(" (PC = %i)",cpu->pc);
    printf(" | instloop %i\n",index);
    index++;

    switch (read_Opcode) {

    case 0x0: // NOP;
        cpu->pc += 1;
        return 4;

    case 0x01: // LD BC,u16
        cpu->bc.value = read_word(cart, cpu->pc + 1);
        cpu->pc += 3;
        return 12;

    case 0x02: // LD (BC),A
        write_memory8(ram, cpu->af.hi, cpu->bc.value);
        cpu->pc += 1;
        return 8;

    case 0x03: // INC BC
        cpu->bc.value++;
        cpu->pc += 1;
        return 8;

    case 0x04: // INC B
        cpu->bc.hi++;
        flags_inc8(cpu, cpu->bc.hi);
        cpu->pc += 1;
        return 4;

    case 0x05: // DEC B
        cpu->bc.hi--;
        flags_dec8(cpu, cpu->bc.hi);
        cpu->pc += 1;
        return 4;

    case 0x06: // LD B,u8
        cpu->bc.hi = read_byte(cart, cpu->pc + 1);
        cpu->pc += 2;
        return 8;

    case 0x07: // RLCA
        {
            uint8_t carry = (cpu->af.hi & 0x80) >> 7;
            cpu->af.hi = (uint8_t)((cpu->af.hi << 1) | carry);
            cpu->zf = false;
            cpu->nf = false;
            cpu->hf = false;
            cpu->cf = carry;
            cpu->pc += 1;
            return 4;
        }

    case 0x08: // LD (u16),SP
        write_memory16(ram, cpu->sp, read_word(cart, cpu->pc + 1));
        cpu->pc += 3;
        return 20;

    case 0x09: // ADD HL,BC
        cpu->hl.value = add_hl16(cpu, cpu->hl.value, cpu->bc.value);
        cpu->pc += 1;
        return 8;

    case 0x0A: // LD A,(BC)
        cpu->af.hi = read_memory8(ram, cpu->bc.value);
        cpu->pc += 1;
        return 8;

    case 0x0B: // DEC BC
        cpu->bc.value--;
        cpu->pc += 1;
        return 8;

    case 0x0C: // INC C
        cpu->bc.lo++;
        flags_inc8(cpu, cpu->bc.lo);
        cpu->pc += 1;
        return 4;

    case 0x0D: // DEC C
        cpu->bc.lo--;
        flags_dec8(cpu, cpu->bc.lo);
        cpu->pc += 1;
        return 4;

    case 0x0E: // LD C,u8
        cpu->bc.lo = read_byte(cart, cpu->pc + 1);
        cpu->pc += 2;
        return 8;

    case 0x0F: // RRCA
        {
            uint8_t carry = cpu->af.hi & 0x01;
            cpu->af.hi = (uint8_t)((cpu->af.hi >> 1) | (carry << 7));
            cpu->zf = false;
            cpu->nf = false;
            cpu->hf = false;
            cpu->cf = carry;
            cpu->pc += 1;
            return 4;
        }

    /* ============================== 0x10 - 0x1F ============================== */

    case 0x10: // STOP
        // TODO : weird instruction quiet rare so i'll do it in the future
        cpu->pc += 2;
        return 4;

    case 0x11: // LD DE,u16
        cpu->de.value = read_word(cart, cpu->pc + 1);
        cpu->pc += 3;
        return 12;

    case 0x12: // LD (DE),A
        write_memory8(ram, cpu->af.hi, cpu->de.value);
        cpu->pc += 1;
        return 8;

    case 0x13: // INC DE
        cpu->de.value++;
        cpu->pc += 1;
        return 8;

    case 0x14: // INC D
        cpu->de.hi++;
        flags_inc8(cpu, cpu->de.hi);
        cpu->pc += 1;
        return 4;

    case 0x15: // DEC D
        cpu->de.hi--;
        flags_dec8(cpu, cpu->de.hi);
        cpu->pc += 1;
        return 4;

    case 0x16: // LD D,u8
        cpu->de.hi = read_byte(cart, cpu->pc + 1);
        cpu->pc += 2;
        return 8;

    case 0x17: // RLA
        {
            uint8_t old_carry = cpu->cf ? 1 : 0;
            uint8_t new_carry = (cpu->af.hi & 0x80) >> 7;
            cpu->af.hi = (uint8_t)((cpu->af.hi << 1) | old_carry);
            cpu->zf = false;
            cpu->nf = false;
            cpu->hf = false;
            cpu->cf = new_carry;
            cpu->pc += 1;
            return 4;
        }

    case 0x18: // JR r8
        {
            int8_t offset = (int8_t)read_byte(cart, cpu->pc + 1);
            cpu->pc += 2;
            cpu->pc = (uint16_t)(cpu->pc + offset);
            return 12;
        }

    case 0x19: // ADD HL,DE
        cpu->hl.value = add_hl16(cpu, cpu->hl.value, cpu->de.value);
        cpu->pc += 1;
        return 8;

    case 0x1A: // LD A,(DE)
        cpu->af.hi = read_memory8(ram, cpu->de.value);
        cpu->pc += 1;
        return 8;

    case 0x1B: // DEC DE
        cpu->de.value--;
        cpu->pc += 1;
        return 8;

    case 0x1C: // INC E
        cpu->de.lo++;
        flags_inc8(cpu, cpu->de.lo);
        cpu->pc += 1;
        return 4;

    case 0x1D: // DEC E
        cpu->de.lo--;
        flags_dec8(cpu, cpu->de.lo);
        cpu->pc += 1;
        return 4;

    case 0x1E: // LD E,u8
        cpu->de.lo = read_byte(cart, cpu->pc + 1);
        cpu->pc += 2;
        return 8;

    case 0x1F: // RRA
        {
            uint8_t old_carry = cpu->cf ? 1 : 0;
            uint8_t new_carry = cpu->af.hi & 0x01;
            cpu->af.hi = (uint8_t)((cpu->af.hi >> 1) | (old_carry << 7));
            cpu->zf = false;
            cpu->nf = false;
            cpu->hf = false;
            cpu->cf = new_carry;
            cpu->pc += 1;
            return 4;
        }

    /* ============================== 0x20 - 0x2F ============================== */

    case 0x20: // JR NZ,r8
        {
            int8_t offset = (int8_t)read_byte(cart, cpu->pc + 1);
            cpu->pc += 2;
            if (!cpu->zf) {
                cpu->pc = (uint16_t)(cpu->pc + offset);
                return 12;
            }
            return 8;
        }

    case 0x21: // LD HL,u16
        cpu->hl.value = read_word(cart, cpu->pc + 1);
        cpu->pc +=3;
        return 12;

    case 0x22: // LD (HL+),A
        write_memory8(ram, cpu->af.hi, cpu->hl.value);
        cpu->hl.value++;
        cpu->pc += 1;
        return 8;

    case 0x23: // INC HL
        cpu->hl.value++;
        cpu->pc += 1;
        return 8;

    case 0x24: // INC H
        cpu->hl.hi++;
        flags_inc8(cpu, cpu->hl.hi);
        cpu->pc += 1;
        return 4;

    case 0x25: // DEC H
        cpu->hl.hi--;
        flags_dec8(cpu, cpu->hl.hi);
        cpu->pc += 1;
        return 4;

    case 0x26: // LD H,u8
        cpu->hl.hi = read_byte(cart, cpu->pc + 1);
        cpu->pc += 2;
        return 8;

    case 0x27: // DAA
        {
            uint8_t a = cpu->af.hi;
            uint8_t correction = 0;
            bool set_carry = cpu->cf;

            if (cpu->hf || (!cpu->nf && (a & 0xF) > 0x9)) {
                correction |= 0x06;
            }
            if (cpu->cf || (!cpu->nf && a > 0x99)) {
                correction |= 0x60;
                set_carry = true;
            }

            a = cpu->nf ? (uint8_t)(a - correction) : (uint8_t)(a + correction);

            cpu->af.hi = a;
            cpu->zf = (a == 0);
            cpu->hf = false;
            cpu->cf = set_carry;
            cpu->pc += 1;
            return 4;
        }

    case 0x28: // JR Z,r8
        {
            int8_t offset = (int8_t)read_byte(cart, cpu->pc + 1);
            cpu->pc += 2;
            if (cpu->zf) {
                cpu->pc = (uint16_t)(cpu->pc + offset);
                return 12;
            }
            return 8;
        }

    case 0x29: // ADD HL,HL
        cpu->hl.value = add_hl16(cpu, cpu->hl.value, cpu->hl.value);
        cpu->pc += 1;
        return 8;

    case 0x2A: // LD A,(HL+)
        cpu->af.hi = read_memory8(ram, cpu->hl.value);
        cpu->hl.value++;
        cpu->pc += 1;
        return 8;

    case 0x2B: // DEC HL
        cpu->hl.value--;
        cpu->pc += 1;
        return 8;

    case 0x2C: // INC L
        cpu->hl.lo++;
        flags_inc8(cpu, cpu->hl.lo);
        cpu->pc += 1;
        return 4;

    case 0x2D: // DEC L
        cpu->hl.lo--;
        flags_dec8(cpu, cpu->hl.lo);
        cpu->pc += 1;
        return 4;

    case 0x2E: // LD L,u8
        cpu->hl.lo = read_byte(cart, cpu->pc + 1);
        cpu->pc += 2;
        return 8;

    case 0x2F: // CPL
        cpu->af.hi = (uint8_t)(~cpu->af.hi);
        cpu->nf = true;
        cpu->hf = true;
        cpu->pc += 1;
        return 4;

    /* ============================== 0x30 - 0x3F ============================== */

    case 0x30: // JR NC,r8
        {
            int8_t offset = (int8_t)read_byte(cart, cpu->pc + 1);
            cpu->pc += 2;
            if (!cpu->cf) {
                cpu->pc = (uint16_t)(cpu->pc + offset);
                return 12;
            }
            return 8;
        }

    case 0x31: //LD SP, u16;
        cpu->sp = read_word(cart, cpu->pc + 1);
        cpu->pc += 3;
        return 12;

    case 0x32: //LD (HL-),A
        write_memory8(ram, cpu->af.hi, cpu->hl.value);
        cpu->hl.value--;
        cpu->pc += 1;
        return 8;

    case 0x33: // INC SP
        cpu->sp++;
        cpu->pc += 1;
        return 8;

    case 0x34: // INC (HL)
        {
            uint8_t result = (uint8_t)(read_memory8(ram, cpu->hl.value) + 1);
            write_memory8(ram, result, cpu->hl.value);
            flags_inc8(cpu, result);
            cpu->pc += 1;
            return 12;
        }

    case 0x35: // DEC (HL)
        {
            uint8_t result = (uint8_t)(read_memory8(ram, cpu->hl.value) - 1);
            write_memory8(ram, result, cpu->hl.value);
            flags_dec8(cpu, result);
            cpu->pc += 1;
            return 12;
        }

    case 0x36: // LD (HL),u8
        write_memory8(ram, read_byte(cart, cpu->pc + 1), cpu->hl.value);
        cpu->pc += 2;
        return 12;

    case 0x37: // SCF
        cpu->nf = false;
        cpu->hf = false;
        cpu->cf = true;
        cpu->pc += 1;
        return 4;

    case 0x38: // JR C,r8
        {
            int8_t offset = (int8_t)read_byte(cart, cpu->pc + 1);
            cpu->pc += 2;
            if (cpu->cf) {
                cpu->pc = (uint16_t)(cpu->pc + offset);
                return 12;
            }
            return 8;
        }

    case 0x39: // ADD HL,SP
        cpu->hl.value = add_hl16(cpu, cpu->hl.value, cpu->sp);
        cpu->pc += 1;
        return 8;

    case 0x3A: // LD A,(HL-)
        cpu->af.hi = read_memory8(ram, cpu->hl.value);
        cpu->hl.value--;
        cpu->pc += 1;
        return 8;

    case 0x3B: // DEC SP
        cpu->sp--;
        cpu->pc += 1;
        return 8;

    case 0x3C: // INC A
        cpu->af.hi++;
        flags_inc8(cpu, cpu->af.hi);
        cpu->pc += 1;
        return 4;

    case 0x3D: // DEC A
        cpu->af.hi--;
        flags_dec8(cpu, cpu->af.hi);
        cpu->pc += 1;
        return 4;

    case 0x3E: // LD A,u8
        cpu->af.hi = read_byte(cart, cpu->pc + 1);
        cpu->pc += 2;
        return 8;

    case 0x3F: // CCF
        cpu->nf = false;
        cpu->hf = false;
        cpu->cf = !cpu->cf;
        cpu->pc += 1;
        return 4;

    /* ============================== 0x40 - 0x7F : LD r,r' ============================== */
    
    case 0x40: cpu->bc.hi = cpu->bc.hi; cpu->pc += 1; return 4; // LD B,B
    case 0x41: cpu->bc.hi = cpu->bc.lo; cpu->pc += 1; return 4; // LD B,C
    case 0x42: cpu->bc.hi = cpu->de.hi; cpu->pc += 1; return 4; // LD B,D
    case 0x43: cpu->bc.hi = cpu->de.lo; cpu->pc += 1; return 4; // LD B,E
    case 0x44: cpu->bc.hi = cpu->hl.hi; cpu->pc += 1; return 4; // LD B,H
    case 0x45: cpu->bc.hi = cpu->hl.lo; cpu->pc += 1; return 4; // LD B,L
    case 0x46: cpu->bc.hi = read_memory8(ram, cpu->hl.value); cpu->pc += 1; return 8; // LD B,(HL)
    case 0x47: cpu->bc.hi = cpu->af.hi; cpu->pc += 1; return 4; // LD B,A

    case 0x48: cpu->bc.lo = cpu->bc.hi; cpu->pc += 1; return 4; // LD C,B
    case 0x49: cpu->bc.lo = cpu->bc.lo; cpu->pc += 1; return 4; // LD C,C
    case 0x4A: cpu->bc.lo = cpu->de.hi; cpu->pc += 1; return 4; // LD C,D
    case 0x4B: cpu->bc.lo = cpu->de.lo; cpu->pc += 1; return 4; // LD C,E
    case 0x4C: cpu->bc.lo = cpu->hl.hi; cpu->pc += 1; return 4; // LD C,H
    case 0x4D: cpu->bc.lo = cpu->hl.lo; cpu->pc += 1; return 4; // LD C,L
    case 0x4E: cpu->bc.lo = read_memory8(ram, cpu->hl.value); cpu->pc += 1; return 8; // LD C,(HL)
    case 0x4F: cpu->bc.lo = cpu->af.hi; cpu->pc += 1; return 4; // LD C,A

    case 0x50: cpu->de.hi = cpu->bc.hi; cpu->pc += 1; return 4; // LD D,B
    case 0x51: cpu->de.hi = cpu->bc.lo; cpu->pc += 1; return 4; // LD D,C
    case 0x52: cpu->de.hi = cpu->de.hi; cpu->pc += 1; return 4; // LD D,D
    case 0x53: cpu->de.hi = cpu->de.lo; cpu->pc += 1; return 4; // LD D,E
    case 0x54: cpu->de.hi = cpu->hl.hi; cpu->pc += 1; return 4; // LD D,H
    case 0x55: cpu->de.hi = cpu->hl.lo; cpu->pc += 1; return 4; // LD D,L
    case 0x56: cpu->de.hi = read_memory8(ram, cpu->hl.value); cpu->pc += 1; return 8; // LD D,(HL)
    case 0x57: cpu->de.hi = cpu->af.hi; cpu->pc += 1; return 4; // LD D,A

    case 0x58: cpu->de.lo = cpu->bc.hi; cpu->pc += 1; return 4; // LD E,B
    case 0x59: cpu->de.lo = cpu->bc.lo; cpu->pc += 1; return 4; // LD E,C
    case 0x5A: cpu->de.lo = cpu->de.hi; cpu->pc += 1; return 4; // LD E,D
    case 0x5B: cpu->de.lo = cpu->de.lo; cpu->pc += 1; return 4; // LD E,E
    case 0x5C: cpu->de.lo = cpu->hl.hi; cpu->pc += 1; return 4; // LD E,H
    case 0x5D: cpu->de.lo = cpu->hl.lo; cpu->pc += 1; return 4; // LD E,L
    case 0x5E: cpu->de.lo = read_memory8(ram, cpu->hl.value); cpu->pc += 1; return 8; // LD E,(HL)
    case 0x5F: cpu->de.lo = cpu->af.hi; cpu->pc += 1; return 4; // LD E,A

    case 0x60: cpu->hl.hi = cpu->bc.hi; cpu->pc += 1; return 4; // LD H,B
    case 0x61: cpu->hl.hi = cpu->bc.lo; cpu->pc += 1; return 4; // LD H,C
    case 0x62: cpu->hl.hi = cpu->de.hi; cpu->pc += 1; return 4; // LD H,D
    case 0x63: cpu->hl.hi = cpu->de.lo; cpu->pc += 1; return 4; // LD H,E
    case 0x64: cpu->hl.hi = cpu->hl.hi; cpu->pc += 1; return 4; // LD H,H
    case 0x65: cpu->hl.hi = cpu->hl.lo; cpu->pc += 1; return 4; // LD H,L
    case 0x66: cpu->hl.hi = read_memory8(ram, cpu->hl.value); cpu->pc += 1; return 8; // LD H,(HL)
    case 0x67: cpu->hl.hi = cpu->af.hi; cpu->pc += 1; return 4; // LD H,A

    case 0x68: cpu->hl.lo = cpu->bc.hi; cpu->pc += 1; return 4; // LD L,B
    case 0x69: cpu->hl.lo = cpu->bc.lo; cpu->pc += 1; return 4; // LD L,C
    case 0x6A: cpu->hl.lo = cpu->de.hi; cpu->pc += 1; return 4; // LD L,D
    case 0x6B: cpu->hl.lo = cpu->de.lo; cpu->pc += 1; return 4; // LD L,E
    case 0x6C: cpu->hl.lo = cpu->hl.hi; cpu->pc += 1; return 4; // LD L,H
    case 0x6D: cpu->hl.lo = cpu->hl.lo; cpu->pc += 1; return 4; // LD L,L
    case 0x6E: cpu->hl.lo = read_memory8(ram, cpu->hl.value); cpu->pc += 1; return 8; // LD L,(HL)
    case 0x6F: cpu->hl.lo = cpu->af.hi; cpu->pc += 1; return 4; // LD L,A

    case 0x70: write_memory8(ram, cpu->bc.hi, cpu->hl.value); cpu->pc += 1; return 8; // LD (HL),B
    case 0x71: write_memory8(ram, cpu->bc.lo, cpu->hl.value); cpu->pc += 1; return 8; // LD (HL),C
    case 0x72: write_memory8(ram, cpu->de.hi, cpu->hl.value); cpu->pc += 1; return 8; // LD (HL),D
    case 0x73: write_memory8(ram, cpu->de.lo, cpu->hl.value); cpu->pc += 1; return 8; // LD (HL),E
    case 0x74: write_memory8(ram, cpu->hl.hi, cpu->hl.value); cpu->pc += 1; return 8; // LD (HL),H
    case 0x75: write_memory8(ram, cpu->hl.lo, cpu->hl.value); cpu->pc += 1; return 8; // LD (HL),L

    case 0x76: // HALT
        cpu->HALTED = 1;
        cpu->pc += 1;
        return 4;

    case 0x77: write_memory8(ram, cpu->af.hi, cpu->hl.value); cpu->pc += 1; return 8; // LD (HL),A

    case 0x78: cpu->af.hi = cpu->bc.hi; cpu->pc += 1; return 4; // LD A,B
    case 0x79: cpu->af.hi = cpu->bc.lo; cpu->pc += 1; return 4; // LD A,C
    case 0x7A: cpu->af.hi = cpu->de.hi; cpu->pc += 1; return 4; // LD A,D
    case 0x7B: cpu->af.hi = cpu->de.lo; cpu->pc += 1; return 4; // LD A,E
    case 0x7C: cpu->af.hi = cpu->hl.hi; cpu->pc += 1; return 4; // LD A,H
    case 0x7D: cpu->af.hi = cpu->hl.lo; cpu->pc += 1; return 4; // LD A,L
    case 0x7E: cpu->af.hi = read_memory8(ram, cpu->hl.value); cpu->pc += 1; return 8; // LD A,(HL)
    case 0x7F: cpu->af.hi = cpu->af.hi; cpu->pc += 1; return 4; // LD A,A

    /* ============================== 0x80 - 0xBF : ALU A,r ============================== */

    case 0x80: flags_add8(cpu, cpu->af.hi, cpu->bc.hi); cpu->af.hi += cpu->bc.hi; cpu->pc += 1; return 4; // ADD A,B
    case 0x81: flags_add8(cpu, cpu->af.hi, cpu->bc.lo); cpu->af.hi += cpu->bc.lo; cpu->pc += 1; return 4; // ADD A,C
    case 0x82: flags_add8(cpu, cpu->af.hi, cpu->de.hi); cpu->af.hi += cpu->de.hi; cpu->pc += 1; return 4; // ADD A,D
    case 0x83: flags_add8(cpu, cpu->af.hi, cpu->de.lo); cpu->af.hi += cpu->de.lo; cpu->pc += 1; return 4; // ADD A,E
    case 0x84: flags_add8(cpu, cpu->af.hi, cpu->hl.hi); cpu->af.hi += cpu->hl.hi; cpu->pc += 1; return 4; // ADD A,H
    case 0x85: flags_add8(cpu, cpu->af.hi, cpu->hl.lo); cpu->af.hi += cpu->hl.lo; cpu->pc += 1; return 4; // ADD A,L
    case 0x86: // ADD A,(HL)
        {
            uint8_t val = read_memory8(ram, cpu->hl.value);
            flags_add8(cpu, cpu->af.hi, val);
            cpu->af.hi += val;
            cpu->pc += 1;
            return 8;
        }
    case 0x87: flags_add8(cpu, cpu->af.hi, cpu->af.hi); cpu->af.hi += cpu->af.hi; cpu->pc += 1; return 4; // ADD A,A

    case 0x88: { uint8_t c = cpu->cf; flags_adc8(cpu, cpu->af.hi, cpu->bc.hi, c); cpu->af.hi = (uint8_t)(cpu->af.hi + cpu->bc.hi + c); cpu->pc += 1; return 4; } // ADC A,B
    case 0x89: { uint8_t c = cpu->cf; flags_adc8(cpu, cpu->af.hi, cpu->bc.lo, c); cpu->af.hi = (uint8_t)(cpu->af.hi + cpu->bc.lo + c); cpu->pc += 1; return 4; } // ADC A,C
    case 0x8A: { uint8_t c = cpu->cf; flags_adc8(cpu, cpu->af.hi, cpu->de.hi, c); cpu->af.hi = (uint8_t)(cpu->af.hi + cpu->de.hi + c); cpu->pc += 1; return 4; } // ADC A,D
    case 0x8B: { uint8_t c = cpu->cf; flags_adc8(cpu, cpu->af.hi, cpu->de.lo, c); cpu->af.hi = (uint8_t)(cpu->af.hi + cpu->de.lo + c); cpu->pc += 1; return 4; } // ADC A,E
    case 0x8C: { uint8_t c = cpu->cf; flags_adc8(cpu, cpu->af.hi, cpu->hl.hi, c); cpu->af.hi = (uint8_t)(cpu->af.hi + cpu->hl.hi + c); cpu->pc += 1; return 4; } // ADC A,H
    case 0x8D: { uint8_t c = cpu->cf; flags_adc8(cpu, cpu->af.hi, cpu->hl.lo, c); cpu->af.hi = (uint8_t)(cpu->af.hi + cpu->hl.lo + c); cpu->pc += 1; return 4; } // ADC A,L
    case 0x8E: // ADC A,(HL)
        {
            uint8_t val = read_memory8(ram, cpu->hl.value);
            uint8_t c = cpu->cf;
            flags_adc8(cpu, cpu->af.hi, val, c);
            cpu->af.hi = (uint8_t)(cpu->af.hi + val + c);
            cpu->pc += 1;
            return 8;
        }
    case 0x8F: { uint8_t c = cpu->cf; flags_adc8(cpu, cpu->af.hi, cpu->af.hi, c); cpu->af.hi = (uint8_t)(cpu->af.hi + cpu->af.hi + c); cpu->pc += 1; return 4; } // ADC A,A

    case 0x90: flags_sub8(cpu, cpu->af.hi, cpu->bc.hi); cpu->af.hi -= cpu->bc.hi; cpu->pc += 1; return 4; // SUB B
    case 0x91: flags_sub8(cpu, cpu->af.hi, cpu->bc.lo); cpu->af.hi -= cpu->bc.lo; cpu->pc += 1; return 4; // SUB C
    case 0x92: flags_sub8(cpu, cpu->af.hi, cpu->de.hi); cpu->af.hi -= cpu->de.hi; cpu->pc += 1; return 4; // SUB D
    case 0x93: flags_sub8(cpu, cpu->af.hi, cpu->de.lo); cpu->af.hi -= cpu->de.lo; cpu->pc += 1; return 4; // SUB E
    case 0x94: flags_sub8(cpu, cpu->af.hi, cpu->hl.hi); cpu->af.hi -= cpu->hl.hi; cpu->pc += 1; return 4; // SUB H
    case 0x95: flags_sub8(cpu, cpu->af.hi, cpu->hl.lo); cpu->af.hi -= cpu->hl.lo; cpu->pc += 1; return 4; // SUB L
    case 0x96: // SUB (HL)
        {
            uint8_t val = read_memory8(ram, cpu->hl.value);
            flags_sub8(cpu, cpu->af.hi, val);
            cpu->af.hi -= val;
            cpu->pc += 1;
            return 8;
        }
    case 0x97: flags_sub8(cpu, cpu->af.hi, cpu->af.hi); cpu->af.hi -= cpu->af.hi; cpu->pc += 1; return 4; // SUB A

    case 0x98: { uint8_t c = cpu->cf; flags_sbc8(cpu, cpu->af.hi, cpu->bc.hi, c); cpu->af.hi = (uint8_t)(cpu->af.hi - cpu->bc.hi - c); cpu->pc += 1; return 4; } // SBC A,B
    case 0x99: { uint8_t c = cpu->cf; flags_sbc8(cpu, cpu->af.hi, cpu->bc.lo, c); cpu->af.hi = (uint8_t)(cpu->af.hi - cpu->bc.lo - c); cpu->pc += 1; return 4; } // SBC A,C
    case 0x9A: { uint8_t c = cpu->cf; flags_sbc8(cpu, cpu->af.hi, cpu->de.hi, c); cpu->af.hi = (uint8_t)(cpu->af.hi - cpu->de.hi - c); cpu->pc += 1; return 4; } // SBC A,D
    case 0x9B: { uint8_t c = cpu->cf; flags_sbc8(cpu, cpu->af.hi, cpu->de.lo, c); cpu->af.hi = (uint8_t)(cpu->af.hi - cpu->de.lo - c); cpu->pc += 1; return 4; } // SBC A,E
    case 0x9C: { uint8_t c = cpu->cf; flags_sbc8(cpu, cpu->af.hi, cpu->hl.hi, c); cpu->af.hi = (uint8_t)(cpu->af.hi - cpu->hl.hi - c); cpu->pc += 1; return 4; } // SBC A,H
    case 0x9D: { uint8_t c = cpu->cf; flags_sbc8(cpu, cpu->af.hi, cpu->hl.lo, c); cpu->af.hi = (uint8_t)(cpu->af.hi - cpu->hl.lo - c); cpu->pc += 1; return 4; } // SBC A,L
    case 0x9E: // SBC A,(HL)
        {
            uint8_t val = read_memory8(ram, cpu->hl.value);
            uint8_t c = cpu->cf;
            flags_sbc8(cpu, cpu->af.hi, val, c);
            cpu->af.hi = (uint8_t)(cpu->af.hi - val - c);
            cpu->pc += 1;
            return 8;
        }
    case 0x9F: { uint8_t c = cpu->cf; flags_sbc8(cpu, cpu->af.hi, cpu->af.hi, c); cpu->af.hi = (uint8_t)(cpu->af.hi - cpu->af.hi - c); cpu->pc += 1; return 4; } // SBC A,A

    case 0xA0: cpu->af.hi &= cpu->bc.hi; flags_and8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // AND B
    case 0xA1: cpu->af.hi &= cpu->bc.lo; flags_and8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // AND C
    case 0xA2: cpu->af.hi &= cpu->de.hi; flags_and8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // AND D
    case 0xA3: cpu->af.hi &= cpu->de.lo; flags_and8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // AND E
    case 0xA4: cpu->af.hi &= cpu->hl.hi; flags_and8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // AND H
    case 0xA5: cpu->af.hi &= cpu->hl.lo; flags_and8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // AND L
    case 0xA6: cpu->af.hi &= read_memory8(ram, cpu->hl.value); flags_and8(cpu, cpu->af.hi); cpu->pc += 1; return 8; // AND (HL)
    case 0xA7: cpu->af.hi &= cpu->af.hi; flags_and8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // AND A

    case 0xA8: cpu->af.hi ^= cpu->bc.hi; flags_xor8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // XOR B
    case 0xA9: cpu->af.hi ^= cpu->bc.lo; flags_xor8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // XOR C
    case 0xAA: cpu->af.hi ^= cpu->de.hi; flags_xor8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // XOR D
    case 0xAB: cpu->af.hi ^= cpu->de.lo; flags_xor8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // XOR E
    case 0xAC: cpu->af.hi ^= cpu->hl.hi; flags_xor8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // XOR H
    case 0xAD: cpu->af.hi ^= cpu->hl.lo; flags_xor8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // XOR L
    case 0xAE: cpu->af.hi ^= read_memory8(ram, cpu->hl.value); flags_xor8(cpu, cpu->af.hi); cpu->pc += 1; return 8; // XOR (HL)
    case 0xAF: // XOR A, A;
        cpu->af.hi = cpu->af.hi ^ cpu->af.hi;
        flags_xor8(cpu, cpu->af.hi);
        cpu->pc++;
        return 4;

    case 0xB0: cpu->af.hi |= cpu->bc.hi; flags_or8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // OR B
    case 0xB1: cpu->af.hi |= cpu->bc.lo; flags_or8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // OR C
    case 0xB2: cpu->af.hi |= cpu->de.hi; flags_or8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // OR D
    case 0xB3: cpu->af.hi |= cpu->de.lo; flags_or8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // OR E
    case 0xB4: cpu->af.hi |= cpu->hl.hi; flags_or8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // OR H
    case 0xB5: cpu->af.hi |= cpu->hl.lo; flags_or8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // OR L
    case 0xB6: cpu->af.hi |= read_memory8(ram, cpu->hl.value); flags_or8(cpu, cpu->af.hi); cpu->pc += 1; return 8; // OR (HL)
    case 0xB7: cpu->af.hi |= cpu->af.hi; flags_or8(cpu, cpu->af.hi); cpu->pc += 1; return 4; // OR A

    case 0xB8: flags_cp8(cpu, cpu->af.hi, cpu->bc.hi); cpu->pc += 1; return 4; // CP B
    case 0xB9: flags_cp8(cpu, cpu->af.hi, cpu->bc.lo); cpu->pc += 1; return 4; // CP C
    case 0xBA: flags_cp8(cpu, cpu->af.hi, cpu->de.hi); cpu->pc += 1; return 4; // CP D
    case 0xBB: flags_cp8(cpu, cpu->af.hi, cpu->de.lo); cpu->pc += 1; return 4; // CP E
    case 0xBC: flags_cp8(cpu, cpu->af.hi, cpu->hl.hi); cpu->pc += 1; return 4; // CP H
    case 0xBD: flags_cp8(cpu, cpu->af.hi, cpu->hl.lo); cpu->pc += 1; return 4; // CP L
    case 0xBE: flags_cp8(cpu, cpu->af.hi, read_memory8(ram, cpu->hl.value)); cpu->pc += 1; return 8; // CP (HL)
    case 0xBF: flags_cp8(cpu, cpu->af.hi, cpu->af.hi); cpu->pc += 1; return 4; // CP A

    /* ============================== 0xC0 - 0xFF ============================== */

    case 0xC0: // RET NZ
        cpu->pc += 1;
        if (!cpu->zf) { cpu->pc = pop16(ram, cpu); return 20; }
        return 8;

    case 0xC1: // POP BC
        cpu->bc.value = pop16(ram, cpu);
        cpu->pc += 1;
        return 12;

    case 0xC2: // JP NZ,u16
        {
            uint16_t addr = read_word(cart, cpu->pc + 1);
            cpu->pc += 3;
            if (!cpu->zf) { cpu->pc = addr; return 16; }
            return 12;
        }

    case 0xC3: // JP u16
        cpu->pc = read_word(cart, cpu->pc + 1);
        return 16;

    case 0xC4: // CALL NZ,u16
        {
            uint16_t addr = read_word(cart, cpu->pc + 1);
            cpu->pc += 3;
            if (!cpu->zf) { push16(ram, cpu, cpu->pc); cpu->pc = addr; return 24; }
            return 12;
        }

    case 0xC5: // PUSH BC
        push16(ram, cpu, cpu->bc.value);
        cpu->pc += 1;
        return 16;

    case 0xC6: // ADD A,u8
        {
            uint8_t val = read_byte(cart, cpu->pc + 1);
            flags_add8(cpu, cpu->af.hi, val);
            cpu->af.hi += val;
            cpu->pc += 2;
            return 8;
        }

    case 0xC7: // RST 00H
        push16(ram, cpu, cpu->pc + 1);
        cpu->pc = 0x00;
        return 16;

    case 0xC8: // RET Z
        cpu->pc += 1;
        if (cpu->zf) { cpu->pc = pop16(ram, cpu); return 20; }
        return 8;

    case 0xC9: // RET
        cpu->pc = pop16(ram, cpu);
        return 16;

    case 0xCA: // JP Z,u16
        {
            uint16_t addr = read_word(cart, cpu->pc + 1);
            cpu->pc += 3;
            if (cpu->zf) { cpu->pc = addr; return 16; }
            return 12;
        }

    case 0xCB: // OP_prefix 0xCB
        {
            uint8_t val = read_byte(cart, cpu->pc + 1);
            return OprefCB(cpu, ram, val);
        }

    case 0xCC: // CALL Z,u16
        {
            uint16_t addr = read_word(cart, cpu->pc + 1);
            cpu->pc += 3;
            if (cpu->zf) { 
                push16(ram, cpu, cpu->pc); cpu->pc = addr; return 24; 
            }
            return 12;
        }

    case 0xCD: // CALL u16
        {
            uint16_t addr = read_word(cart, cpu->pc + 1);
            cpu->pc += 3;
            push16(ram, cpu, cpu->pc);
            cpu->pc = addr;
            return 24;
        }

    case 0xCE: // ADC A,u8
        {
            uint8_t val = read_byte(cart, cpu->pc + 1);
            uint8_t c = cpu->cf;
            flags_adc8(cpu, cpu->af.hi, val, c);
            cpu->af.hi = (uint8_t)(cpu->af.hi + val + c);
            cpu->pc += 2;
            return 8;
        }

    case 0xCF: // RST 08H
        push16(ram, cpu, cpu->pc + 1);
        cpu->pc = 0x08;
        return 16;

    case 0xD0: // RET NC
        cpu->pc += 1;
        if (!cpu->cf) { cpu->pc = pop16(ram, cpu); return 20; }
        return 8;

    case 0xD1: // POP DE
        cpu->de.value = pop16(ram, cpu);
        cpu->pc += 1;
        return 12;

    case 0xD2: // JP NC,u16
        {
            uint16_t addr = read_word(cart, cpu->pc + 1);
            cpu->pc += 3;
            if (!cpu->cf) { cpu->pc = addr; return 16; }
            return 12;
        }

    // 0xD3 : unset

    case 0xD4: // CALL NC,u16
        {
            uint16_t addr = read_word(cart, cpu->pc + 1);
            cpu->pc += 3;
            if (!cpu->cf) { push16(ram, cpu, cpu->pc); cpu->pc = addr; return 24; }
            return 12;
        }

    case 0xD5: // PUSH DE
        push16(ram, cpu, cpu->de.value);
        cpu->pc += 1;
        return 16;

    case 0xD6: // SUB u8
        {
            uint8_t val = read_byte(cart, cpu->pc + 1);
            flags_sub8(cpu, cpu->af.hi, val);
            cpu->af.hi -= val;
            cpu->pc += 2;
            return 8;
        }

    case 0xD7: // RST 10H
        push16(ram, cpu, cpu->pc + 1);
        cpu->pc = 0x10;
        return 16;

    case 0xD8: // RET C
        cpu->pc += 1;
        if (cpu->cf) { cpu->pc = pop16(ram, cpu); return 20; }
        return 8;

    case 0xD9: // RETI
        cpu->IME = true;
        cpu->pc = pop16(ram, cpu);
        return 16;

    case 0xDA: // JP C,u16
        {
            uint16_t addr = read_word(cart, cpu->pc + 1);
            cpu->pc += 3;
            if (cpu->cf) { cpu->pc = addr; return 16; }
            return 12;
        }

    // 0xDB : unset

    case 0xDC: // CALL C,u16
        {
            uint16_t addr = read_word(cart, cpu->pc + 1);
            cpu->pc += 3;
            if (cpu->cf) { push16(ram, cpu, cpu->pc); cpu->pc = addr; return 24; }
            return 12;
        }

    // 0xDD : unset

    case 0xDE: // SBC A,u8
        {
            uint8_t val = read_byte(cart, cpu->pc + 1);
            uint8_t c = cpu->cf;
            flags_sbc8(cpu, cpu->af.hi, val, c);
            cpu->af.hi = (uint8_t)(cpu->af.hi - val - c);
            cpu->pc += 2;
            return 8;
        }

    case 0xDF: // RST 18H
        push16(ram, cpu, cpu->pc + 1);
        cpu->pc = 0x18;
        return 16;

    case 0xE0: // LDH (u8),A  ->  (0xFF00 + u8)
        write_memory8(ram, cpu->af.hi, (uint16_t)(0xFF00 + read_byte(cart, cpu->pc + 1)));
        cpu->pc += 2;
        return 12;

    case 0xE1: // POP HL
        cpu->hl.value = pop16(ram, cpu);
        cpu->pc += 1;
        return 12;

    case 0xE2: // LD (C),A  ->  (0xFF00 + C)
        write_memory8(ram, cpu->af.hi, (uint16_t)(0xFF00 + cpu->bc.lo));
        cpu->pc += 1;
        return 8;

    // 0xE3, 0xE4 : unset

    case 0xE5: // PUSH HL
        push16(ram, cpu, cpu->hl.value);
        cpu->pc += 1;
        return 16;

    case 0xE6: // AND u8
        cpu->af.hi &= read_byte(cart, cpu->pc + 1);
        flags_and8(cpu, cpu->af.hi);
        cpu->pc += 2;
        return 8;

    case 0xE7: // RST 20H
        push16(ram, cpu, cpu->pc + 1);
        cpu->pc = 0x20;
        return 16;

    case 0xE8: // ADD SP,r8
        {
            int8_t offset = (int8_t)read_byte(cart, cpu->pc + 1);
            flags_sp_r8(cpu, cpu->sp, offset);
            cpu->sp = (uint16_t)(cpu->sp + offset);
            cpu->pc += 2;
            return 16;
        }

    case 0xE9: // JP (HL)
        cpu->pc = cpu->hl.value;
        return 4;

    case 0xEA: // LD (u16),A
        write_memory8(ram, cpu->af.hi, read_word(cart, cpu->pc + 1));
        cpu->pc += 3;
        return 16;

    // 0xEB, 0xEC, 0xED : unset

    case 0xEE: // XOR u8
        cpu->af.hi ^= read_byte(cart, cpu->pc + 1);
        flags_xor8(cpu, cpu->af.hi);
        cpu->pc += 2;
        return 8;

    case 0xEF: // RST 28H
        push16(ram, cpu, cpu->pc + 1);
        cpu->pc = 0x28;
        return 16;

    case 0xF0: // LDH A,(u8)  ->  (0xFF00 + u8)
        cpu->af.hi = read_memory8(ram, (uint16_t)(0xFF00 + read_byte(cart, cpu->pc + 1)));
        cpu->pc += 2;
        return 12;

    case 0xF1: // POP AF
        {
            uint16_t popped = pop16(ram, cpu);
            cpu->af.value = (uint16_t)(popped & 0xFFF0);
            cpu->zf = (popped & 0x80) != 0;
            cpu->nf = (popped & 0x40) != 0;
            cpu->hf = (popped & 0x20) != 0;
            cpu->cf = (popped & 0x10) != 0;
            cpu->pc += 1;
            return 12;
        }

    case 0xF2: // LD A,(C)  ->  (0xFF00 + C)
        cpu->af.hi = read_memory8(ram, (uint16_t)(0xFF00 + cpu->bc.lo));
        cpu->pc += 1;
        return 8;

    case 0xF3: // DI
        cpu->IME = 0;
        cpu->InterruptionPending = 0;
        return 4;

    // 0xF4 : unset

    case 0xF5: // PUSH AF
        {
            uint8_t f = (uint8_t)((cpu->zf << 7) | (cpu->nf << 6) | (cpu->hf << 5) | (cpu->cf << 4));
            push16(ram, cpu, (uint16_t)((cpu->af.hi << 8) | f));
            cpu->pc += 1;
            return 16;
        }

    case 0xF6: // OR u8
        cpu->af.hi |= read_byte(cart, cpu->pc + 1);
        flags_or8(cpu, cpu->af.hi);
        cpu->pc += 2;
        return 8;

    case 0xF7: // RST 30H
        push16(ram, cpu, cpu->pc + 1);
        cpu->pc = 0x30;
        return 16;

    case 0xF8: // LD HL,SP+r8
        {
            int8_t offset = (int8_t)read_byte(cart, cpu->pc + 1);
            flags_sp_r8(cpu, cpu->sp, offset);
            cpu->hl.value = (uint16_t)(cpu->sp + offset);
            cpu->pc += 2;
            return 12;
        }

    case 0xF9: // LD SP,HL
        cpu->sp = cpu->hl.value;
        cpu->pc += 1;
        return 8;

    case 0xFA: // LD A,(u16)
        cpu->af.hi = read_memory8(ram, read_word(cart, cpu->pc + 1));
        cpu->pc += 3;
        return 16;

    case 0xFB: // EI
        cpu->InterruptionPending = 1;
        cpu->pc += 1;
        return 4;

    // 0xFC, 0xFD : unset

    case 0xFE: // CP u8
        flags_cp8(cpu, cpu->af.hi, read_byte(cart, cpu->pc + 1));
        cpu->pc += 2;
        return 8;

    case 0xFF: // RST 38H
        push16(ram, cpu, cpu->pc + 1);
        cpu->pc = 0x38;
        return 16;

    default:
        printf("-Opcodes not found : %02X\n",read_Opcode);
        cpu->pc += 3;
        return 4;
    }
}  
