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

        struct Sprite{
            uint8_t y_position;
            uint8_t tile_index;
            uint8_t attributes;
            uint8_t x_position;
        };
        
        // Memory
        std::array<uint8_t, 2048> vram;
        std::array<uint8_t, 32> palette;

        std::array<uint8_t, 256> oam;
        std::array<Sprite, 8> secondary_oam;
        uint8_t primary_oam_index = 0;

        // Sprite rendering state
        std::array<uint8_t, 8> sprite_pattern_low = {};
        std::array<uint8_t, 8> sprite_pattern_high = {};
        std::array<uint8_t, 8> sprite_attribute = {};
        std::array<uint8_t, 8> sprite_x_counter = {};
        uint8_t sprite_count = 0;
        bool sprite_zero_hit= false;
    

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
            0xFF666666, 0xFF882A00, 0xFFA71214, 0xFFA4003B, 0xFF7E005C, 0xFF40006E, 0xFF00066C, 0xFF001D56,
            0xFF003533, 0xFF00480B, 0xFF005200, 0xFF084F00, 0xFF4D4000, 0xFF000000, 0xFF000000, 0xFF000000,
            0xFFADADAD, 0xFFD95F15, 0xFFFF4042, 0xFFFE2775, 0xFFCC1AA0, 0xFF7B1EB7, 0xFF2031B5, 0xFF004E99,
            0xFF006D6B, 0xFF008738, 0xFF00930C, 0xFF328F00, 0xFF8D7C00, 0xFF000000, 0xFF000000, 0xFF000000,
            0xFFFFFEFF, 0xFFFFB064, 0xFFFF9092, 0xFFFF76C6, 0xFFFF6AF3, 0xFFCC6EFE, 0xFF7081FE, 0xFF229EEA,
            0xFF00BEBC, 0xFF00D888, 0xFF30E45C, 0xFF82E045, 0xFFDECD48, 0xFF4F4F4F, 0xFF000000, 0xFF000000,
            0xFFFFFEFF, 0xFFFFDFC0, 0xFFFFD2D3, 0xFFFFC8E8, 0xFFFFC2FB, 0xFFEAC4FE, 0xFFC5CCFE, 0xFFA5D8F7,
            0xFF94E5E4, 0xFF96EFCF, 0xFFABF4BD, 0xFFCCF3B3, 0xFFF2EBB5, 0xFFB8B8B8, 0xFF000000, 0xFF000000
        };

        // Internal read/write
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);
        uint16_t apply_mirroring(uint16_t addr);
        void extract_pixel();
};