// PPU stuff
#ifndef PPU_H
#define PPU_H
#include <cstdint>
#include <cstddef>
#include "memory.h"

class PPUBus;

struct PPUCtrl {
    //  Vblank NMI enable (0: off, 1: on)
    char V: 1; 
    // PPU master/slave select
    //(0: read backdrop from EXT pins; 1: output color on EXT pins)
    char P: 1;
    // Sprite size (0: 8x8 pixels; 1: 8x16 pixels – see PPU OAM#Byte 1)
    char H: 1; 
    // Background pattern table address (0: $0000; 1: $1000)
    char B: 1; 
    // Sprite pattern table address for 8x8 sprites
    //  (0: $0000; 1: $1000; ignored in 8x16 mode)
    char S: 1; 
    // VRAM address increment per CPU read/write of PPUDATA
    // (0: add 1, going across; 1: add 32, going down)
    char I: 1;
    // base name table address
    // (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
    char Nx: 1; 
    char Ny: 1;

};

struct PPUMask { 
    // Emphasize blue
    char B: 1;
    // Emphasize green (red on PAL/Dendy)
    char G: 1;
    // Emphasize red (green on PAL/Dendy)
    char R: 1;
    // 1: Enable sprite rendering
    char s: 1;
    // 1: Enable background rendering
    char b: 1;
    // 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
    char M: 1;
    // 1: Show background in leftmost 8 pixels of screen, 0: Hide
    char m: 1;
    // Greyscale (0: normal color, 1: greyscale)
    char Gs: 1;
};
    
struct PPUStatus{
    // Vblank flag, cleared on read
    char V: 1;
    // Sprite 0 hit flag
    char S: 1;
    // Sprite overflow flag
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