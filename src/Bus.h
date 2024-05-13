#ifndef Bus_H
#define Bus_H
#include <cstdint>
#include "p6502.h"

class P6502;

class Bus
{
    public:

        Bus(); // Constructor

        P6502* processor; // CPU component

        uint8_t ram[64 * 1024];

        uint8_t read(uint16_t addr);

        void write(uint16_t addr, uint8_t data);

        
};
#endif
