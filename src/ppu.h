// PPU stuff
#ifndef PPU_H
#define PPU_H
#include <cstdint>
#include <cstddef>
#include "memory.h"

class PPUBus;

struct PPUCtrl {
    char V: 1; 
    char P: 1;
    char H: 1; 
    char B: 1; 
    
    char S: 1; 
    char I: 1;
    char Nx: 1; 
    char Ny: 1;

};

struct PPUMask { 
    char B: 1;
    char G: 1;
    char R: 1;
    char s: 1;
    
    char b: 1;
    char M: 1;
    char m: 1;
    char Gs: 1;
};
    
struct PPUStatus{
    char V: 1;
    char S: 1;
    char O: 1;
};




class PPU: public IMemory{
    private: 
        uint8_t vram[16 * 1024];
        uint8_t spr_ram[256];

    
    public:
        PPUBus* ppu_bus;
        PPU(PPUBus* ppu_bus);
        PPUCtrl ppu_ctrl;
        PPUMask ppu_mask;
        PPUStatus ppu_status;
        uint8_t oam_addr;
        uint8_t oam_data;
        uint16_t ppu_scroll;
        uint16_t ppu_addr;
        uint16_t ppu_data;

        uint8_t oamd_ma;

        uint8_t read(uint16_t addr) override;
        void write(uint16_t addr, uint8_t data) override;

};

#endif