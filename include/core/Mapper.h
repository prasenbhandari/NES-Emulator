#pragma once

#include <cstdint>
#include <vector>

class Mapper {
    protected:
        uint8_t prg_rom_banks;
        uint8_t chr_rom_banks;
        std::vector<uint8_t> &prg_rom;
        std::vector<uint8_t> &chr_rom;

    public:
        Mapper(uint8_t prg_rom_banks, uint8_t chr_rom_banks, std::vector<uint8_t> &prg_rom, std::vector<uint8_t> &chr_rom);
        ~Mapper();

        virtual uint8_t cpu_read(uint16_t addr) = 0;
        virtual void cpu_write(uint16_t addr, uint8_t data) = 0;

        virtual uint8_t ppu_read(uint16_t addr) = 0;
        virtual void ppu_write(uint16_t addr, uint8_t data) = 0;
};