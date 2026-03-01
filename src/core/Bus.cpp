#include "core/Bus.h"
#include "core/PPU.h"
#include "core/CPU.h"

Bus::Bus() {
    cpu_ram.fill(0x00);   
}

uint8_t Bus::read(uint16_t addr) {
    if (addr >= 0x0000 && addr < 0x2000) {
        return cpu_ram[addr & 0x7FF];
    } else if (addr >= 0x2000 && addr < 0x4000) {
        return ppu->cpu_read(addr);
    } else if (addr == 0x4016){
        uint8_t data = 0x00;
        if(is_latching){
            data = (controller_state & 0x80) ? 1 : 0;
        } else {
            data = (controller_shift & 0x80) ? 1 : 0;
            controller_shift <<= 1;
        }
        return data;
    } else if (addr == 0x4017){
        return 0x00;
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
        ppu->cpu_write(addr, data);
    } else if (addr == 0x4014) {
        // OAM DMA
        uint16_t dma_base = data * 0x100;
        for (int i = 0; i < 256; ++i) {
            uint8_t byte = read(dma_base + i);
            ppu->cpu_write(0x2004, byte);
        }
        dma_stall_cycles = 513 + (cpu->get_total_cycles() % 2 == 0 ? 0 : 1);
    } else if (addr == 0x4016) {
        if (data & 0x01) {
            is_latching = true;
        } else {
            is_latching = false;
            controller_shift = controller_state;
        }
    } else if (addr >= 0x4000 && addr < 0x4018) {
        // APU stub - ignore writes for now
    } else if (addr >= 0x8000 && addr <= 0xFFFF){
        cartridge->cpu_write(addr, data);
    }
}


