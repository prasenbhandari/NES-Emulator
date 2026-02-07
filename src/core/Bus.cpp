#include "core/Bus.h"

Bus::Bus() {
    cpu_ram.fill(0x00);   
}

uint8_t Bus::read(uint16_t addr, bool b_read_only) {
    if (addr >= 0x0000 && addr < 0x2000) {
        return cpu_ram[addr & 0x7FF];
    }

    return 0x00;
}

void Bus::write(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr < 0x2000) {
        cpu_ram[addr & 0x7FF] = data;
    }
}


