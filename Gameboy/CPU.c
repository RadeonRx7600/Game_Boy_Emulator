#include<stdio.h>
#include<stdint.h>

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

    bool z;
    bool n;
    bool h;
    bool c;

    bool HALTED;
    bool PendingInterruptEnabled;
    bool IME;

} Registers;
