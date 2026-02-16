#pragma once

#include <cstdint>
#include <array>

#include "core/Cartridge.h"
#include "core/Mapper.h"

class PPU {
    public:
        bool nmi_occurred = false;
        bool frame_complete = false;

        PPU();

        // CPU interface
        uint8_t cpu_read(uint16_t addr);
        void cpu_write(uint16_t addr, uint8_t data);

        void connect_cartridge(Cartridge* cartridge);
        void clock();

    
    private:
        Cartridge* cartridge = nullptr;
        
        // Memory
        std::array<uint8_t, 2048> vram;
        std::array<uint8_t, 32> palette;
        std::array<uint8_t, 256> oam;

        struct Sprite{
            uint8_t y_position;
            uint8_t tile_index;
            uint8_t attributes;
            uint8_t x_position;
        };

        std::array<Sprite, 8> secondary_oam;
        // Registers
        std::array<uint8_t, 8> registers;
        uint8_t fine_x = 0x00;

        // Addressing
        uint16_t vram_address = 0x0000;
        uint16_t temp_vram_address = 0x0000;

        bool address_latch = false;

        // Masks
        static constexpr uint16_t MASK_COARSE_X = 0x001F;
        static constexpr uint16_t MASK_COARSE_Y = 0x03E0;
        static constexpr uint16_t MASK_NAMETABLE = 0x0C00;
        static constexpr uint16_t MASK_FINE_Y = 0x7000;

        // Data buffer for PPUDATA reads
        uint8_t data_buffer = 0x00;

        // Mirroring mode
        MirrorMode mirror_mode = MirrorMode::HORIZONTAL;

        int16_t scanline = -1;
        uint16_t cycle = 0;
        uint64_t frames = 0;


        // Internal read/write
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);
            uint16_t apply_mirroring(uint16_t addr);
};