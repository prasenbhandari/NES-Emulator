#include "core/PPU.h"

PPU::PPU() {
    registers.fill(0);
}

void PPU::connect_cartridge(Cartridge* cart) {
    cartridge = cart;
    if (cartridge && cartridge->get_mapper()) {
        mirror_mode = cartridge->get_mapper()->get_mirror_mode();
    }
}

uint8_t PPU::cpu_read(uint16_t addr) {

    uint8_t reg = addr & 0x0007;
    uint8_t data = 0x00;
    switch (reg) {
        case 0x0002: // PPUSTATUS
            data = registers[reg] & 0xE0;
            
            registers[reg] &= 0x7F; // Clear the VBlank flag

            address_latch = false;
            break;
        
        case 0x0004: // OAMDATA
            data = oam[registers[0x0003]];
            break;

        case 0x0007: // PPUDATA
            data = data_buffer;
            data_buffer = read(vram_address);

            if(vram_address >= 0x3F00){
                data = data_buffer;     // TODO: May need to implment palette quirk
            }
            vram_address += (registers[0x0000] & 0x04) ? 32 : 1;
            break;
    }

    return data;
}

void PPU::cpu_write(uint16_t addr, uint8_t data) {

    uint8_t reg = addr & 0x0007;

    switch (reg){
        case 0x0000: // PPUCTRL
            registers[reg] = data;

            temp_vram_address &= ~MASK_NAMETABLE;
            temp_vram_address |= (data & 0x03) << 10;
            break;

        case 0x0001: // PPUMASK
            registers[reg] = data;
            break;

        case 0x0003: // OAMADDR
            registers[reg] = data;
            break;
        
        case 0x0004: // OAMDATA
            oam[registers[0x0003]] = data;
            registers[0x0003]++;
            break;

        case 0x0005:{ // PPUSCROLL
            if (!address_latch){
                fine_x = data & 0x07;

                temp_vram_address &= ~MASK_COARSE_X;
                temp_vram_address |= (data >> 3) & MASK_COARSE_X;
            }else{
                // Set coarse Y (bits 5-9) from bits 3-7 of data
                temp_vram_address &= ~MASK_COARSE_Y;
                temp_vram_address |= ((data >> 3) & 0x1F) << 5;

                // Set fine Y (bits 12-14) from bits 0-2 of data
                temp_vram_address &= ~MASK_FINE_Y;
                temp_vram_address |= (data & 0x07) << 12;
            }
            address_latch = !address_latch;
            break;
        }

        case 0x0006:{ // PPUADDR
            if (!address_latch){
                temp_vram_address = (temp_vram_address & 0x00FF) | static_cast<uint16_t>((data & 0x3F) << 8);
            }else{
                temp_vram_address = (temp_vram_address & 0xFF00) | data;
                vram_address = temp_vram_address;
            }
            address_latch = !address_latch;
            break;
        }

        case 0x0007: // PPUDATA
            write(vram_address, data);
            vram_address += (registers[0x0000] & 0x04) ? 32 : 1;
            break;
    }
}


uint16_t PPU::apply_mirroring(uint16_t addr) {
    if (addr < 0x2000 || addr >= 0x3000) {
        return addr;
    }

    // Nametable address bits: $2ABC where A=bit10, B=bit11, C=bits 0-9
    // Nametable layout:
    //   0 ($2000) | 1 ($2400)
    //   ----------+----------
    //   2 ($2800) | 3 ($2C00)
    
    uint16_t nt_addr = addr & 0x0FFF;
    
    if (mirror_mode == MirrorMode::HORIZONTAL) {
        // Horizontal: rows are mirrored
        // $2000-$23FF same as $2400-$27FF (clear bit 10)
        // $2800-$2BFF same as $2C00-$2FFF (clear bit 10)
        nt_addr &= 0xFBFF;  // Clear bit 11
    } else if (mirror_mode == MirrorMode::VERTICAL) {
        // Vertical: columns are mirrored
        // $2000-$23FF same as $2800-$2BFF (clear bit 11)
        // $2400-$27FF same as $2C00-$2FFF (clear bit 11)
        nt_addr &= 0xF7FF;  // Clear bit 10
    }
    
    return 0x2000 + nt_addr;
}

uint8_t PPU::read(uint16_t addr) {
    addr &= 0x3FFF;
    
    if (addr < 0x2000) {
        // Pattern tables from cartridge
        return cartridge->ppu_read(addr);

    } else if (addr >= 0x2000 && addr < 0x3000) {
        // Nametables with mirroring applied
        addr = apply_mirroring(addr);
        return vram[addr & 0x07FF];

    } else if (addr >= 0x3000 && addr < 0x3F00) {
        // mirror to the nametable
        addr -= 0x1000; // Mirror of nametables
        addr = apply_mirroring(addr);
        return vram[addr & 0x07FF];

    } else if (addr >= 0x3F00 && addr < 0x4000) {
        // Palette with mirroring
        uint16_t palette_addr = addr & 0x001F;
        // Mirror $3F10/$3F14/$3F18/$3F1C to $3F00/$3F04/$3F08/$3F0C
        if (palette_addr == 0x10 || palette_addr == 0x14 || 
            palette_addr == 0x18 || palette_addr == 0x1C) {
            palette_addr -= 0x10;
        }
        return palette[palette_addr];
    }
    
    return 0x00;
}

void PPU::write(uint16_t addr, uint8_t data) {
    addr &= 0x3FFF;
    
    if (addr < 0x2000) {
        cartridge->ppu_write(addr, data);

    } else if (addr >= 0x2000 && addr < 0x3000) {
        // Nametables with mirroring applied
        addr = apply_mirroring(addr);
        vram[addr & 0x07FF] = data;

    } else if (addr >= 0x3000 && addr < 0x3F00) {
        // Mirror of nametables
        addr -= 0x1000; // Mirror of nametables
        addr = apply_mirroring(addr);
        vram[addr & 0x07FF] = data;

    } else if (addr >= 0x3F00 && addr < 0x4000) {
        // Palette with mirroring
        uint16_t palette_addr = addr & 0x001F;
        // Mirror $3F10/$3F14/$3F18/$3F1C to $3F00/$3F04/$3F08/$3F0C
        if (palette_addr == 0x10 || palette_addr == 0x14 || 
            palette_addr == 0x18 || palette_addr == 0x1C) {
            palette_addr -= 0x10;
        }
        palette[palette_addr] = data;
    }
}

void PPU::clock() {

    if ((scanline < 240 || scanline == 261) && (cycle >= 1 && cycle <= 256)) {
        switch (cycle % 8) {
            

        }
    }   

    cycle++;
    if (cycle >= 341) {
        cycle = 0;
        scanline++;

        if (scanline >= 261) {
            scanline = -1;
            frames++;
            frame_complete = true;
        }
    }

    if (scanline == 241 && cycle == 1) {
        registers[0x0002] |= 0x80; // Set VBlank flag

        if(registers[0x0000] & 0x80)
            nmi_occurred = true;
    }

    if (scanline == -1 && cycle == 1) {
        registers[0x0002] &= ~0x80; // Clear VBlank flag
    }

    if (scanline == 261 && cycle == 339){
        if (frames % 2 == 1) {
            // Skip cycle on odd frames when rendering is enabled
            if ((registers[0x0001] & 0x18) != 0) {
                cycle = 0;
                scanline = 0;
                frames++;
            }
        }
    }
}