#include "core/MMC0.h"

MMC0::MMC0(uint8_t prg_rom_banks, uint8_t chr_rom_banks, std::vector<uint8_t> &prg_rom, std::vector<uint8_t> &chr_rom) 
    : Mapper(prg_rom_banks, chr_rom_banks, prg_rom, chr_rom) {
    
}


MMC0::~MMC0() {

}


uint8_t MMC0::cpu_read(uint16_t addr) {
    return prg_rom[addr & (prg_rom_banks > 1 ? 0x7FFF : 0x3FFF)];
}

void MMC0::cpu_write(uint16_t addr, uint8_t data) {
    // TODO
}

uint8_t MMC0::ppu_read(uint16_t addr) {
    return chr_rom[addr];
}

void MMC0::ppu_write(uint16_t addr, uint8_t data) {
    // No-op
}
