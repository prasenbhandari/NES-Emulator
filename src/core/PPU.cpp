#include "core/PPU.h"

PPU::PPU() {
    registers.fill(0);
    vram_address = 0x0000;
    oam.fill(0xFF);
}

void PPU::connect_cartridge(Cartridge* cart) {
    cartridge = cart;
    if (cartridge && cartridge->get_mapper()) {
        mirror_mode = cartridge->get_mapper()->get_mirror_mode();
    }
}

void PPU::reset(){
    registers.fill(0);
    vram_address = 0x0000;
    temp_vram_address = 0x0000;
    address_latch = false;
    fine_x = 0;
    data_buffer = 0;
    scanline = 0;
    cycle = 0;
    oam.fill(0xFF);
    sprite_count = 0;
    sprite_pattern_low.fill(0x00);
    sprite_pattern_high.fill(0x00);
    sprite_x_counter.fill(0xFF);
    sprite_attribute.fill(0x00);
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
            if (vram_address >= 0x3F00) {
                data = read(vram_address);
                
                if (registers[0x0001] & 0x01) {
                    data &= 0xF0;
                }
                
                uint16_t shadowed_addr = vram_address - 0x1000;
                data_buffer = read(shadowed_addr);
            } else {
                data = data_buffer;
                data_buffer = read(vram_address);
            }
            
            vram_address += (registers[0x0000] & 0x04) ? 32 : 1;
            break;
    }

    return data;
}

void PPU::cpu_write(uint16_t addr, uint8_t data) {

    uint8_t reg = addr & 0x0007;

    switch (reg){
        case 0x0000: { // PPUCTRL
            bool nmi_enabled_before = registers[reg] & 0x80;
            bool nmi_enabled_now = data & 0x80;
            
            registers[reg] = data;

            temp_vram_address &= ~MASK_NAMETABLE;
            temp_vram_address |= (data & 0x03) << 10;
            
            if (!nmi_enabled_before && nmi_enabled_now && (registers[0x0002] & 0x80)) {
                nmi_occurred = true;
            }
            break;
        }

        case 0x0001: // PPUMASK
            registers[reg] = data;
            break;

        case 0x0003: // OAMADDR
            registers[reg] = data;
            break;
        
        case 0x0004:{ // OAMDATA
            // Ignore writes during rendering (scanlines 0-239 and pre-render 261)
            bool rendering = (scanline < 240 || scanline == 261) && ((registers[0x0001] & 0x18) != 0);
            if (!rendering) {
                oam[registers[0x0003]] = data;
                registers[0x0003]++;
            }
            break;
        }   

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

void PPU::extract_pixel(){
    if (scanline >= 240) return;
    
    uint8_t bg_pixel = 0x00;
    uint8_t bg_palette = 0x00;

    if (registers[0x0001] & 0x08) { // Background rendering enabled
        uint16_t bit_mux = 0x8000 >> fine_x;

        uint8_t p0_pixel = (bg_pattern_shift_low & bit_mux) ? 1 : 0;
        uint8_t p1_pixel = (bg_pattern_shift_high & bit_mux) ? 1 : 0;
        bg_pixel = (p1_pixel << 1) | p0_pixel;

        uint8_t a0_bit = (bg_attribute_shift_low & bit_mux) ? 1 : 0;
        uint8_t a1_bit = (bg_attribute_shift_high & bit_mux) ? 1 : 0;
        bg_palette = (a1_bit << 1) | a0_bit;
    }

    // Check left-edge clipping (bit 1 of PPUMASK: show background in leftmost 8 pixels)
    if (!(registers[0x0001] & 0x02) && cycle <= 8) {
        bg_pixel = 0;  // Clip to backdrop
        bg_palette = 0;
    }

    if (bg_pixel == 0) {
        bg_palette = 0; // Use universal background color for palette 0
    }

    uint8_t sp_pixel = 0x00;
    uint8_t sp_palette = 0x00;
    uint8_t sp_priority = 0x00;
    bool    sp_is_zero = false;

    if (registers[0x0001] & 0x10) {
        for (uint8_t i = 0; i < sprite_count; i++) {
            if (sprite_x_counter[i] != 0) continue;

            uint8_t lo = (sprite_pattern_low[i] & 0x80) >> 7;
            uint8_t hi = (sprite_pattern_high[i] & 0x80) >> 6;
            uint8_t pixel = hi | lo;

            if (pixel != 0) {
                sp_pixel = pixel;
                sp_palette = (sprite_attribute[i] & 0x03) + 4;
                sp_priority = (sprite_attribute[i] >> 5) & 0x01;
                sp_is_zero = (i == 0) && sprite_zero_hit;
                break;
            }
        }
    }

    // Left-edge sprite clipping (PPUMASK bit 2)
    if (!(registers[0x0001] & 0x04) && cycle <= 8) {
        sp_pixel = 0;
    }

    uint8_t  final_pixel = 0x00;
    uint8_t  final_palette = 0x00;

    if (bg_pixel == 0 && sp_pixel == 0) {
        final_pixel = 0;
        final_palette = 0;
    } else if (bg_pixel == 0 && sp_pixel != 0) {
        final_pixel = sp_pixel;
        final_palette = sp_palette;
    } else if (bg_pixel != 0 && sp_pixel == 0) {
        final_pixel = bg_pixel;
        final_palette = bg_palette;
    } else {
        if (sp_priority == 0) {
            final_pixel = sp_pixel;
            final_palette = sp_palette;
        } else {
            final_pixel = bg_pixel;
            final_palette = bg_palette;
        }

        // Sprite 0 hit detection
        if (sp_is_zero &&
            (registers[0x0001] & 0x08) &&
            (registers[0x0001] & 0x10) &&
            cycle != 256) {
            bool bg_left_clip = !(registers[0x0001] & 0x02);
            bool sp_left_clip = !(registers[0x0001] & 0x04);
            if ((!bg_left_clip && !sp_left_clip) || cycle > 8) {
                registers[0x0002] |= 0x40;
            }
        }
    }

    uint8_t palette_index = read(0x3F00 + (final_palette << 2) + final_pixel) & 0x3F;
    
    // Apply greyscale mode
    if (registers[0x0001] & 0x01) {
        palette_index &= 0x30;
    }
    
    uint32_t pixel_index = (scanline * 256) + (cycle - 1);
    frame_buffer[pixel_index] = NES_PALETTE[palette_index];
}

void PPU::clock() {
    bool rendering_enabled = (registers[0x0001] & 0x18) != 0;

    if (rendering_enabled && (scanline < 240 || scanline == 261) && 
        ((cycle >= 1 && cycle <= 256) || (cycle >= 321 && cycle <= 336))) {
        switch ((cycle) % 8) {
            case 0:{
                bg_pattern_shift_low = (bg_pattern_shift_low & 0xFF00) | bg_next_tile_lsb;
                bg_pattern_shift_high = (bg_pattern_shift_high & 0xFF00) | bg_next_tile_msb;

                uint8_t palette_id = bg_next_tile_attribute;

                if (vram_address & 0x02) palette_id >>= 2;
                if (vram_address & 0x40) palette_id >>= 4;

                palette_id &= 0x03;

                bg_attribute_shift_low = (bg_attribute_shift_low & 0xFF00) | ((palette_id & 0x01) ? 0xFF : 0x00);
                bg_attribute_shift_high = (bg_attribute_shift_high & 0xFF00) | ((palette_id & 0x02) ? 0xFF : 0x00);

                if((vram_address & MASK_COARSE_X) == 31){
                    vram_address &= ~MASK_COARSE_X;
                    vram_address ^= 0x0400;
                }else{
                    vram_address++;
                }

                break;
            }
            case 1:
                bg_next_tile_id = read(0x2000 | (vram_address & 0x0FFF));
                break;

            case 3:
                bg_next_tile_attribute = read(0x23C0 | (vram_address & 0x0C00) | ((vram_address >> 4) & 0x38) | ((vram_address >> 2) & 0x07));
                break;

            case 5: {
                uint16_t pattern_addr = (registers[0x0000] & 0x10 ? 0x1000 : 0x0000) + (bg_next_tile_id * 16) + (vram_address >> 12);
                bg_next_tile_lsb = read(pattern_addr);
                break;
            }
            case 7: {
                uint16_t pattern_addr = (registers[0x0000] & 0x10 ? 0x1000 : 0x0000) + (bg_next_tile_id * 16) + (vram_address >> 12) + 8;
                bg_next_tile_msb = read(pattern_addr);
                break;
            }
        }

        if (cycle == 256){
            if((vram_address & MASK_FINE_Y) != MASK_FINE_Y){
                vram_address += 0x1000; // Increment fine Y
            }else{
                vram_address &= ~MASK_FINE_Y; // Reset fine Y

                uint16_t coarse_y = (vram_address & MASK_COARSE_Y) >> 5;
                if (coarse_y == 29) {
                    coarse_y = 0;
                    vram_address ^= 0x0800; // Switch vertical nametable
                } else if (coarse_y == 31) {
                    coarse_y = 0;
                } else {
                    coarse_y++;
                }

                vram_address = (vram_address & ~MASK_COARSE_Y) | (coarse_y << 5);
            }
        }
    }

    // Clear secondary OAM at the start of sprite evaluation
    if (rendering_enabled && (scanline < 240 || scanline == 261) && cycle == 65){
        secondary_oam.fill({0xFF, 0xFF, 0xFF, 0xFF});
    }

    // Sprite Evaluation
    if (rendering_enabled && (scanline < 240 || scanline == 261) && cycle == 257){
        sprite_count = 0;
        sprite_zero_hit = false;

        for(int i = 0; i < 64; i++){
            uint8_t y_pos = oam[i * 4];
            uint8_t sprite_height = (registers[0x0000] & 0x20) ? 16 : 8;

            if (scanline >= y_pos && scanline < y_pos + sprite_height) {
                if (sprite_count < 8){
                    secondary_oam[sprite_count] = {y_pos, oam[i * 4 + 1], oam[i * 4 + 2], oam[i * 4 + 3]};
                    if (i == 0) {
                        sprite_zero_hit = true;
                    }
                    sprite_count++;
                } else {
                    registers[0x0002] |= 0x20; // Set sprite overflow flag
                    break;
                }
            }
        }
    }

    if (rendering_enabled && (scanline < 240 || scanline == 261) && cycle >= 257 && cycle <= 320){
        uint8_t slot = (cycle - 257) / 8;
        uint8_t cycle_offset = (cycle - 257) % 8;

        if (slot < sprite_count) {
            const Sprite& sp = secondary_oam[slot];
            bool flip_h = sp.attributes & 0x40;
            bool flip_v = sp.attributes & 0x80;

            if (cycle_offset == 0) {
                sprite_attribute[slot] = sp.attributes;
                sprite_x_counter[slot] = sp.x_position;
            } else if (cycle_offset == 4 || cycle_offset == 6) {
                uint16_t addr = 0x0000;
                bool is_8x16 = registers[0x0000] & 0x20;

                if (!is_8x16) {
                    uint16_t table_base = (registers[0x0000] & 0x08) ? 0x1000 : 0x0000;
                    uint8_t tile_row = static_cast<uint8_t>(scanline - sp.y_position);
                    if (flip_v) tile_row = 7 - tile_row;
                    addr = table_base + (sp.tile_index * 16) + tile_row;
                } else {
                    uint16_t table_base = (sp.tile_index & 0x01) ? 0x1000 : 0x0000;
                    uint8_t  tile_top = sp.tile_index & 0xFE;
                    uint8_t  tile_row = static_cast<uint8_t>(scanline - sp.y_position);
                    if (flip_v) tile_row = 15 - tile_row;
                    if (tile_row >= 8) { 
                        tile_row -= 8;
                        tile_top++;
                    }
                    addr = table_base + (tile_top * 16) + tile_row;
                }

                auto flip_byte = [](uint8_t b) -> uint8_t {
                    b = static_cast<uint8_t>((b & 0xF0) >> 4 | (b & 0x0F) << 4);
                    b = static_cast<uint8_t>((b & 0xCC) >> 2 | (b & 0x33) << 2);
                    b = static_cast<uint8_t>((b & 0xAA) >> 1 | (b & 0x55) << 1);
                    return b;
                };

                if (cycle_offset == 4) {
                    uint8_t lo = read(addr);
                    sprite_pattern_low[slot] = flip_h ? flip_byte(lo) : lo;
                } else {
                    uint8_t hi = read(addr + 8);
                    sprite_pattern_high[slot] = flip_h ? flip_byte(hi) : hi;
                }
            }
        } else {
            if (cycle_offset == 0) {
                sprite_attribute[slot] = 0x00;
                sprite_x_counter[slot] = 0xFF;
            } else if (cycle_offset == 4 || cycle_offset == 6) {
                uint16_t dummy_addr = (registers[0x0000] & 0x08) ? 0x1FF0 : 0x0FF0;
                if(cycle_offset == 4) {
                    read(dummy_addr);
                    sprite_pattern_low[slot] = 0x00;
                } else {
                    read(dummy_addr + 8);
                    sprite_pattern_high[slot] = 0x00;
                }
            }
        }
    }

    if (rendering_enabled && (scanline < 240 || scanline == 261) && cycle == 257){
        vram_address = (vram_address & ~0x041F) | (temp_vram_address & 0x041F);
    }

    if (rendering_enabled && (scanline < 240 || scanline == 261) && 
        (cycle >= 257 && cycle <= 320)) {
        registers[0x0003] = 0;
    }

    if (scanline < 240 && cycle >= 1 && cycle <= 256) {
        extract_pixel();
    }

    if (rendering_enabled && (scanline < 240 || scanline == 261) && 
        ((cycle >= 1 && cycle <= 256) || (cycle >= 321 && cycle <= 336))) {
        
        bg_pattern_shift_low <<= 1;
        bg_pattern_shift_high <<= 1;
        bg_attribute_shift_low <<= 1;
        bg_attribute_shift_high <<= 1;
    }

    if (rendering_enabled && scanline < 240 && cycle >= 1 && cycle <= 256) {
        for (uint8_t i = 0; i < 8; i++) {
            if (sprite_x_counter[i] > 0) {
                sprite_x_counter[i]--;
            } else {
                sprite_pattern_low[i]  <<= 1;
                sprite_pattern_high[i] <<= 1;
            }
        }
    }

    if (rendering_enabled && (scanline < 240 || scanline == 261) && 
        (cycle >= 257 && cycle <= 320)) {
        if (cycle % 8 == 1) {
            read(0x2000 | (vram_address & 0x0FFF));
        }
    }

    if (rendering_enabled && (scanline < 240 || scanline == 261) && 
        (cycle == 337 || cycle == 339)) {
        read(0x2000 | (vram_address & 0x0FFF));
    }   

    if (scanline == 241 && cycle == 1) {
        registers[0x0002] |= 0x80; // Set VBlank flag

        if(registers[0x0000] & 0x80)
            nmi_occurred = true;
    }

    if (scanline == 261 && cycle == 1) {
        registers[0x0002] &= ~0xE0; // Clear VBlank, sprite 0 hit, and sprite overflow flags
    }

    if (scanline == 261 && cycle >= 280 && cycle <= 304){
        vram_address = (vram_address & ~0x7BE0) | (temp_vram_address & 0x7BE0);
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
    
    cycle++;
    if (cycle >= 341) {
        cycle = 0;
        scanline++;

        if (scanline > 261) {
            scanline = 0;
            frames++;
            frame_complete = true;
        }
    }
}