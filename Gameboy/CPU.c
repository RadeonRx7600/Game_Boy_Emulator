#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>

#define IF_ADDR 0xFF0F
#define IE_ADDR 0xFFFF

#define _VBLANK 0x40
#define _LCD 0x48
#define _TIMER 0x50
#define _SERIAL 0x58
#define _JOYPAD 0x60

#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07

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

    uint16_t sp;
    uint16_t pc;

    bool zf;
    bool nf;
    bool hf;
    bool cf;

    bool HALTED;
    bool PendingInterruptEnabled;
    bool IME;

} Registers;

uint8_t check_interrupts(Registers *cpu, RAM *ram) {

    uint8_t IE = read_memory8(ram, IE_ADDR);
    uint8_t IF = read_memory8(ram, IF_ADDR);
    uint8_t pending = IE & IF;

    if (cpu->InterruptionPending == 1) { // EI instruction
        cpu->IME = 1;
        cpu->InterruptionPending = 0;
        return 0;
    }
    if (pending == 0) { // if any intpt occured while HALTED then the HALTED state stops 
        return 0;
    }

    if (cpu->HALTED == 1) {  
        cpu->HALTED = 0;
    }
 
    if (cpu->IME == 0) {
        return 0;
    }
    
    uint8_t bit;
    uint16_t ISR; // Interrupt Service Routine

    if (pending & 0x01) { // bit 0 : VBlank
        bit = 0x01;
        ISR = _VBLANK;
    } else if (pending & 0x02) { // bit 1 : LCD STAT
        bit = 0x02;
        ISR = _LCD;
    } else if (pending & 0x04) { // bit 2 : Timer
        bit = 0x04;
        ISR = _TIMER;
    } else if (pending & 0x08) { // bit 3 : Serial
        bit = 0x08;
        ISR = _SERIAL;
    } else { // bit 4 : Joypad
        bit = 0x10;
        ISR = _JOYPAD;
    }

    cpu->IME = false; // while ISR, IME is false and RETI will set IME true again at the end of ISR
    write_memory8(ram, (uint8_t)(IF & ~bit), IF_ADDR); // erase the bit

    push16(ram, cpu, cpu->pc); // for RETI
    cpu->pc = ISR;

    return 20;
}

void init_GB(Registers *cpu, RAM *ram) {
    
    // Gameboy init (http://bgb.bircd.org/pandocs.htm#powerupsequence)

    //CPU init --;
    cpu->af.value = 432;
    cpu->bc.value = 19;
    cpu->de.value = 216;
    cpu->hl.value = 333;
    cpu->pc = 100;
    cpu->sp = 65534;

    //Memory init --;
    ram->Memory[0xFF05] = 0x00;
	ram->Memory[0xFF06] = 0x00;
	ram->Memory[0xFF07] = 0x00;
	ram->Memory[0xFF0F] = 0xE1; // not specified
	ram->Memory[0xFF10] = 0x80;
	ram->Memory[0xFF11] = 0xBF;
	ram->Memory[0xFF12] = 0xF3;
	ram->Memory[0xFF14] = 0xBF;
	ram->Memory[0xFF16] = 0x3F;
	ram->Memory[0xFF17] = 0x00;
	ram->Memory[0xFF19] = 0xBF;
	ram->Memory[0xFF1A] = 0x7F;
	ram->Memory[0xFF1B] = 0xFF;
	ram->Memory[0xFF1C] = 0x9F;
	ram->Memory[0xFF1E] = 0xBF;
	ram->Memory[0xFF20] = 0xFF;
	ram->Memory[0xFF21] = 0x00;
	ram->Memory[0xFF22] = 0x00;
	ram->Memory[0xFF23] = 0xBF;
	ram->Memory[0xFF24] = 0x77;
	ram->Memory[0xFF25] = 0xF3;
	ram->Memory[0xFF26] = 0xF1;
	ram->Memory[0xFF40] = 0x91;
	ram->Memory[0xFF42] = 0x00;
	ram->Memory[0xFF43] = 0x00;
	ram->Memory[0xFF45] = 0x00;
	ram->Memory[0xFF47] = 0xFC;
	ram->Memory[0xFF48] = 0xFF;
	ram->Memory[0xFF49] = 0xFF;
	ram->Memory[0xFF4A] = 0x00;
	ram->Memory[0xFF4B] = 0x00;
	ram->Memory[0xFFFF] = 0x00;
}

void print_binary(uint8_t n) {
    for (int i = sizeof(n) * 8 - 1; i >= 0; i--)
        printf("%d", (n >> i) & 1);
}

void dump_state_Logs(Registers *cpu ,RAM *ram) {
	printf("===========================");
	printf("AF = %i\n",cpu->af.value);
	printf("BC = %i\n",cpu->bc.value);
	printf("DE = %i\n",cpu->de.value);
	printf("HL = %i\n",cpu->hl.value);
	printf("PC = %i\n",cpu->pc);
	printf("SP = %i\n",cpu->sp);

	printf("\n===========================");
	printf("Ram[IE 0xFFFF] = ");
	print_binary(ram->Memory[0xFFFF]);

	printf("\nRam[IF 0xFF0F] = ");
	print_binary(ram->Memory[0xFF0F]);
}
