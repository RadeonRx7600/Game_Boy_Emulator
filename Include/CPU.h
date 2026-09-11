#ifndef CPU_H
#define CPU_H

#include<stdint.h>
#include<stdbool.h>

#include"/home/juan/Bureau/eco-system/Code/C,C#/Game_Boy_Emu/include/Memory.h"

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

    bool InterruptionPending;
    bool HALTED;
    bool IME;

} Registers;

typedef struct {
    uint16_t div_cycles;
    uint16_t tima_cycles;
} Timer;

void init_GB(Registers *cpu, RAM *ram);

void update_timer(Timer *timer, RAM *ram, uint8_t cycles);

uint8_t check_interrupts(Registers *cpu, RAM *ram);

void print_binary(uint8_t n);

void dump_state_Logs(Registers *cpu ,RAM *ram);

#endif
