#include "platform/Platform.h"

void Platform::init_window(int width, int height) {
    SetTargetFPS(60);
    window_width = width * scale_factor;
    window_height = height * scale_factor;
    InitWindow(window_width, window_height, "NES Emulator");
}

void Platform::handle_input() {
    if (WindowShouldClose()) {
        close_window = true;
    }
}

void Platform::render_frame(const std::array<uint32_t, 256 * 240>& frame_buffer) {
    BeginDrawing();
    ClearBackground(BLACK);

    for (int y = 0; y < 240; y++) {
        for (int x = 0; x < 256; x++) {
            uint32_t rgb = frame_buffer[y * 256 + x];
            uint8_t r = (rgb >> 16) & 0xFF;
            uint8_t g = (rgb >> 8) & 0xFF;
            uint8_t b = rgb & 0xFF;
            Color color = { r, g, b, 255 };
            DrawRectangle(x * scale_factor, y * scale_factor, scale_factor, scale_factor, color);
        }
    }

    EndDrawing();
}

bool Platform::should_close() const {
    return close_window;
}

