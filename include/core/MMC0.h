#pragma once

#include "core/Mapper.h"
#include <vector>
#include <cstdint>

class MMC0 : public Mapper {
    public:
        MMC0(uint8_t prg_rom_banks, uint8_t chr_rom_banks, std::vector<uint8_t> &prg_rom, std::vector<uint8_t> &chr_rom);
        ~MMC0();

        uint8_t cpu_read(uint16_t addr) override;
        void cpu_write(uint16_t addr, uint8_t data) override;

        uint8_t ppu_read(uint16_t addr) override;
        void ppu_write(uint16_t addr, uint8_t data) override;
};
