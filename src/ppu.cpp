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
    cycles = 0;

    tile_index = 0;
    attr_byte = 0;
    pattern_low = 0;
    pattern_high = 0;

}

void PPU::cycle() {
    // Increment the cycle counter
    cycles++;

    // Calculate current scanline (0-261)
    int scanline = cycles / 341;
    int cycle_in_line = cycles % 341; // 0-340
   


    // -----------------------------
    // 1. VBlank handling
    // -----------------------------
    if (scanline == 241 && cycle_in_line == 1) {
        // Enter VBlank
        ppu_status.flags |= 0x80; // set VBlank flag
        // NMI triggering is handled outside
    }

    if (scanline == 261 && cycle_in_line == 1) {
        // Pre-render scanline: clear VBlank and Sprite 0 Hit
        ppu_status.flags &= ~0x80; // clear VBlank
        ppu_status.flags &= ~0x40; // clear Sprite 0 Hit
    }

    // -----------------------------
    // 2. Visible scanlines + pre-render
    // -----------------------------
    bool rendering_scanline = (scanline <= 239 || scanline == 261);
    if (rendering_scanline) {

        // -----------------------------
        // 2a. Background fetch / tile pipeline
        // -----------------------------
        if (cycle_in_line >= 1 && cycle_in_line <= 256) {
            // Each 8-cycle block is one tile fetch
            int tile_cycle = (cycle_in_line - 1) % 8;

            switch (tile_cycle) {
                case 1: {
                    // Fetch Name Table byte
                    uint16_t name_table_addr = 0x2000 | (v & 0x0FFF);
                    tile_index = vram[name_table_addr];
                    // Load into tile latch (shift registers would use this)
                    break;
                }
                case 3: {
                    // Fetch Attribute Table byte
                    uint16_t attr_addr = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
                    attr_byte = vram[attr_addr];
                    // Prepare palette info
                    break;
                }
                case 5: {
                    // Fetch Pattern Table Low byte
                    uint16_t pattern_addr = (ppu_ctrl.B ? 0x1000 : 0x0000) + tile_index * 16 + ((v >> 12) & 0x7);
                    pattern_low = vram[pattern_addr];
                    break;
                }
                case 7: {
                    // Fetch Pattern Table High byte
                    uint16_t pattern_addr = (ppu_ctrl.B ? 0x1000 : 0x0000) + tile_index * 16 + ((v >> 12) & 0x7) + 8;
                    pattern_high = vram[pattern_addr];
                    break;
                }
            }
            // put the pixels into the shift_registers

      if (tile_cycle == 7) {
            // Load new tile pattern bytes into the lower 8 bits of the shift registers
            bg_pattern_shift_low  &= 0xFF00;             // Clear lower 8 bits
            bg_pattern_shift_low  |= (uint16_t)pattern_low;  // Load new low pattern byte

            bg_pattern_shift_high &= 0xFF00;             
            bg_pattern_shift_high |= (uint16_t)pattern_high; 

            uint8_t pal_lo = (attr_byte & 1) ? 0xFF : 0x00;  
            uint8_t pal_hi = (attr_byte & 2) ? 0xFF : 0x00;  

            bg_attr_shift_low  &= 0xFF00;
            bg_attr_shift_low  |= (uint16_t)pal_lo;

            bg_attr_shift_high &= 0xFF00;
            bg_attr_shift_high |= (uint16_t)pal_hi;
        }


            // -----------------------------
            // Increment horizontal (coarse X) every 8 cycles
            // -----------------------------
            if (tile_cycle == 7) {
                if ((v & 0x001F) == 31) {
                    // Wrap coarse X, switch horizontal nametable
                    v &= ~0x001F;    // coarse X = 0
                    v ^= 0x0400;     // toggle horizontal nametable bit
                } else {
                    v += 1;          // increment coarse X
                }

            }
        }

        // -----------------------------
        // 2b. End of visible line: increment vertical / fine Y
        // -----------------------------
        if (cycle_in_line == 256) {
            // Increment fine Y
            if ((v & 0x7000) != 0x7000) {
                v += 0x1000; // fine Y += 1
            } else {
                v &= ~0x7000; // fine Y = 0
                int y = (v & 0x03E0) >> 5; // coarse Y
                if (y == 29) {
                    y = 0;
                    v ^= 0x0800; // switch vertical nametable
                } else if (y == 31) {
                    y = 0; // overflow, no nametable switch
                } else {
                    y++;
                }
                v = (v & ~0x03E0) | (y << 5);
            }
        }

        // -----------------------------
        // 2c. Cycle 257: horizontal scroll copy
        // -----------------------------
        if (cycle_in_line == 257) {
            // Copy coarse X and horizontal nametable from t -> v
            v = (v & 0x7BE0) | (t & 0x041F);
        }

        // -----------------------------
        // 2d. Cycles 321–336: prefetch next scanline
        // -----------------------------
        if (cycle_in_line >= 321 && cycle_in_line <= 336) {
            // Fetch two tiles of next scanline (same pattern as above)
            int tile_cycle = (cycle_in_line - 321) % 8;
            // repeat name/attribute/pattern fetches into next scanline latches
        }

        // -----------------------------
        // 2e. Cycles 337–340: dummy fetches
        // -----------------------------
        if (cycle_in_line >= 337 && cycle_in_line <= 340) {
            // Fetch ignored name table bytes to keep alignment
        }
    }

    // -----------------------------
    // 3. End of frame
    // -----------------------------
    if (cycles >= 262 * 341) {
        cycles = 0; // wrap to start of new frame
    }
}

// setting a particular bit 
// k = 0010 0000
 //    0000 1000
// k |= (1 << 4);  
// ppu contains 8 registers exposed to the cpu memory
uint8_t PPU::read(uint16_t addr){  
    // check addr values between 0-7 to see which register is being reference 
    // beteen $2000- $2007
    uint8_t mirrored_addr = addr & 0x0007;
    switch (mirrored_addr){
        case 2:
            {
                w = 0;
                uint8_t value = ppu_status.flags;
                ppu_status.flags &= ~0x80;  // clear VBlank
                return value;
            }
        case 4: 
            {

                return oam_data;

            }

        case 7: 
        {   
            // return value of previous contents
            // returns value of read buffer 
            // updated on every ppudata read 
            // 1. store prev value from read buffer, 
            // 2. update read buffer with new value read
            // 3. return prev value that was stored
            uint8_t read_buffer;
            if(ppu_addr < 0x3F00){
                read_buffer = ppu_data;
                ppu_data = vram[ppu_addr];
                                
            }
            else{
                read_buffer = vram[ppu_addr]; // palette addr is read directly
                ppu_data = vram[ppu_addr - 0x1000];
            }
            ppu_addr += (ppu_ctrl.flags & (1 << 2)) ? 32 : 1;
            ppu_addr &= 0x3FFF;
            return read_buffer;
 
        }
    }

    return vram[addr];

}

void PPU::write(uint16_t addr, uint8_t data){
    uint8_t mirrored_addr = addr & 0x0007;
    switch(mirrored_addr){
        // case 4 appears for both read and write since the oam_data register can be both read and written to 
        case 0:
        {
            ppu_ctrl.flags  = data;

        }
        
        case 1:
        {
            ppu_mask.flags = data;

        }
        case 4:
        {
            return;

        }
        case 5:
        {
            return;

        }
        case 7:{
            vram[ppu_addr] = data;
            ppu_addr += (ppu_ctrl.flags & (1 << 2)) ? 32 : 1;
            ppu_addr &= 0x3FFF;
        }

    }
    
    return;
}


void PPU::generate_background() {
    const int tile_width = 8;
    const int tile_height = 8;

    
}


// mental model of how all the ppu nametables, ppu pattern tables,
// ppu attribute tables


// ppu name tables 
// 32x30 grid of indexes into the pattern table


// pattern tables
// contains the shape of each background tile and sprite
// have to look at the 16 bytes, make them into 2 
// separate planes and then use the low 8 bytes and high 8 bytes
// to make 2 bit planes

// example
//    0-7: 18 38 18 18 18 18 7E 00
//    8-F: 00 00 00 00 00 00 00 00

// 18 : 0 0 0 1 1 0 0 0 
// 38 : 0 0 1 1 1 0 0 0 
// 18 : 0 0 0 1 1 0 0 0 
// 18 : 0 0 0 1 1 0 0 0 
// 18 : 0 0 0 1 1 0 0 0 
// 18 : 0 0 0 1 1 0 0 0 
// 7E : 0 1 1 1 1 1 1 0
// 00 : 0 0 0 0 0 0 0 0


// Note that the above using the indexes to the pattern table
// generate the number 1. Similarly the other 8x8 byte table that is 
// made is just in the same light but its all 0s as of now
