#include<stdio.h>
#include<stdint.h>

/*
Bit | Name | Explanation
-----------------------------------
7	|   z  | Zero flag
6	|   n  | Subtraction flag (BCD)
5	|   h  | Half Carry flag  (BCD)
4	|   c  | Carry flag
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

    uint16_t sp;
    uint16_t pc;

    bool z;
    bool n;
    bool h;
    bool c;

} Registers;
