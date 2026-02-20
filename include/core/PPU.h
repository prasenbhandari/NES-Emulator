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
        void reset();

        const std::array<uint32_t, 256 * 240>& get_frame_buffer() const { return frame_buffer; }

    
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

        uint16_t scanline = 0;
        uint16_t cycle = 0;
        uint64_t frames = 0;

        // Shift registers for background rendering
        uint16_t bg_pattern_shift_low = 0x0000;
        uint16_t bg_pattern_shift_high = 0x0000;
        uint16_t bg_attribute_shift_low = 0x00;
        uint16_t bg_attribute_shift_high = 0x00;

        // Attribute latch
        uint8_t bg_next_tile_id = 0x00;
        uint8_t bg_next_tile_attribute = 0x00;
        uint8_t bg_next_tile_lsb = 0x00;
        uint8_t bg_next_tile_msb = 0x00;

        // Frame buffer
        std::array<uint32_t, 256 * 240> frame_buffer = {0};

        const std::array<uint32_t, 64> NES_PALETTE = {
            0x626262, 0x001FB2, 0x0015EE, 0x3B0DC4, 0x6D07BC, 0x721B6D, 0x682310, 0x543D00,
            0x2D5500, 0x005400, 0x004F08, 0x003A20, 0x002E55, 0x000000, 0x000000, 0x000000,
            0xAEAEAE, 0x0D57FF, 0x404FFF, 0x7B2FFF, 0xBE29FF, 0xC946FF, 0xC05E48, 0x9B7100,
            0x6B9100, 0x2BA100, 0x009924, 0x008A4F, 0x007B9C, 0x000000, 0x000000, 0x000000,
            0xFFFFFF, 0x0FB7FF, 0x5B9FFF, 0x9B7FFF, 0xF379FF, 0xFF7BFF, 0xFF9C6D, 0xFFB347,
            0xDCC93D, 0x9FD900, 0x4FDF4F, 0x1FD77F, 0x3FCCC7, 0x000000, 0x000000, 0x000000,
            0xFFFFFF, 0xA7E7FF, 0xC7D7FF, 0xD7C7FF, 0xF7C7FF, 0xFFC7FF, 0xFFD7BF, 0xFFE7A7,
            0xF7F797, 0xD7FF7F, 0xB7FF9F, 0x9FFFBF, 0xA7FFD7, 0xC7C7C7, 0x000000, 0x000000
        };

        // Internal read/write
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);
        uint16_t apply_mirroring(uint16_t addr);
        void extract_pixel();
};