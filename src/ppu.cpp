#include "ppu.h"
#include "ppuBus.h"

PPU::PPU(){
    // set internal registers initial state
    v = 0x0000;
    t = 0x0000;
    x = 0x00;
    w = 0x00;

    // set external registers initial state
    ppu_ctrl.flags = 0x0000;
    ppu_mask.flags = 0x0000;
    ppu_status.flags = 0x0000;
    oam_addr = 0x00;
    
    ppu_gen_latch = 0;
    ppu_scroll = 0x0000;
    ppu_addr = 0x0000;
    ppu_data = 0x00;
    
    oam_data = 0;
    oam_dma = 0;


}
// setting a particular bit 
// k = 0010 0000
 //    0000 1000
// k |= (1 << 4);  
// ppu contains 8 registers exposed to the cpu memory
uint8_t PPU::read(uint16_t addr){  
    // check addr values between 0-7 to see which register is being reference 
    // beteen $2000- $2007
    switch (addr){
        case 2:
            w = 0;
            uint8_t value = ppu_status.flags;
            ppu_status.flags &= ~0x80;  // clear VBlank
            return value;
        
        // case 4: return oam_data;
        
        // case 6: return ppu_data;
    }

    return vram[addr];

}

void PPU::write(uint16_t addr, uint8_t data){
    switch(addr){
        case 4:
            return;
    }
    
    return;
}


void PPU::generate_background(){
    return;
}
