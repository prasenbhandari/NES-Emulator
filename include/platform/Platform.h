#pragma once

#include <array>

#include <cstdint>
#include <raylib.h>

class Platform {
    public:
        void init_window(int width, int height);
        void handle_input(uint8_t& controller_state);
        void render_frame(const std::array<uint32_t, 256 * 240>& frame_buffer);
        bool should_close() const;

    private:
        bool close_window = false;
        int window_width = 0;
        int window_height = 0;
        int scale_factor = 3; // Scale factor for rendering

        Texture2D screen_texture;
        Image screen_image;
};