#include "core/Bus.h"

Bus::Bus() {
    cpu_ram.fill(0x00);   
}

uint8_t Bus::read(uint16_t addr) {
    if (addr >= 0x0000 && addr < 0x2000) {
        return cpu_ram[addr & 0x7FF];
    } else if (addr >= 0x2000 && addr < 0x4000) {
        return 0x80;  // PPU stub - return 0x80 so bit 7 is set (prevents infinite wait loops)
    } else if (addr >= 0x4000 && addr < 0x4018) {
        return 0x00;  // APU stub
    } else if (addr >= 0x8000 && addr <= 0xFFFF) {
        return cartridge->cpu_read(addr);
    }

    return 0x00;
}

void Bus::write(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr < 0x2000) {
        cpu_ram[addr & 0x7FF] = data;
    } else if (addr >= 0x2000 && addr < 0x4000) {
        // PPU stub - ignore writes for now
    } else if (addr >= 0x4000 && addr < 0x4018) {
        // APU stub - ignore writes for now
    } else if (addr >= 0x8000 && addr <= 0xFFFF){
        cartridge->cpu_write(addr, data);
    }
    
}


