#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "core/Mapper.h"

class Cartridge{
    private:
        std::vector<uint8_t> prg_rom;
        std::vector<uint8_t> chr_rom;
        std::vector<uint8_t> prg_ram;
        uint8_t mapper_id;
        Mapper* mapper = nullptr;


    public:
        Cartridge();
        ~Cartridge();

        struct INESHeader {
            char name[4];
            uint8_t prg_rom_chunks;
            uint8_t chr_rom_chunks;
            uint8_t mapper1;
            uint8_t mapper2;
            uint8_t prg_ram_size;
            char unused[7];
        };

        INESHeader header;

        bool load(const std::string &filename);

        uint8_t cpu_read(uint16_t addr);
        void cpu_write(uint16_t addr, uint8_t data);

        uint8_t ppu_read(uint16_t addr);
        void ppu_write(uint16_t addr, uint8_t data);
};
