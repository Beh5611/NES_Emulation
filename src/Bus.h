#ifndef BUS_H
#define BUS_H
#include <cstdint>
#include <iostream>
#include <vector>
#include "p6502.h"
#include "memory.h"

class Bus
{
    private:
        RAM ram;
        PPU ppu;

    public:        

        
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);

        
};
#endif
