#include "ppu.h"
#include "ppuBus.h"
PPU::PPU(PPUBus* ppu_bus){
    this->ppu_bus = ppu_bus;

}
uint8_t PPU::read(uint16_t addr){  
    uint8_t data = vram[addr];
    return data;

}

void PPU::write(uint16_t addr, uint8_t data){
    vram[addr] = data;
}

