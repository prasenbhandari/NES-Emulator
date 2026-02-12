#include "core/Mapper.h"

Mapper::Mapper(uint8_t prg_rom_banks, uint8_t chr_rom_banks, std::vector<uint8_t> &prg_rom, std::vector<uint8_t> &chr_rom)
    : prg_rom_banks(prg_rom_banks), chr_rom_banks(chr_rom_banks), prg_rom(prg_rom), chr_rom(chr_rom) {
}

Mapper::~Mapper() {

}