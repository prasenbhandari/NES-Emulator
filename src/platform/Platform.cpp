#include "platform/Platform.h"

void Platform::init_window(int width, int height) {
    SetTargetFPS(60);
    window_width = width * scale_factor;
    window_height = height * scale_factor;
    InitWindow(window_width, window_height, "NES Emulator");

    screen_image = GenImageColor(256, 240, BLACK);
    screen_texture = LoadTextureFromImage(screen_image);
}

void Platform::handle_input(uint8_t& controller_state) {
    if (WindowShouldClose()) {
        close_window = true;
    }

    controller_state = 0x00;

    if (IsKeyDown(KEY_Z)) controller_state |= 0x80; // A
    if (IsKeyDown(KEY_X)) controller_state |= 0x40; // B
    if (IsKeyDown(KEY_SPACE)) controller_state |= 0x20; // Select
    if (IsKeyDown(KEY_ENTER)) controller_state |= 0x10; // Start
    if (IsKeyDown(KEY_UP)) controller_state |= 0x08; // Up
    if (IsKeyDown(KEY_DOWN)) controller_state |= 0x04; // Down
    if (IsKeyDown(KEY_LEFT)) controller_state |= 0x02; // Left
    if (IsKeyDown(KEY_RIGHT)) controller_state |= 0x01; // Right
}

void Platform::render_frame(const std::array<uint32_t, 256 * 240>& frame_buffer) {
    UpdateTexture(screen_texture, frame_buffer.data());
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTextureEx(screen_texture, {0, 0}, 0.0f, static_cast<float>(scale_factor), WHITE);

    DrawFPS(10,10);
    EndDrawing();
}

bool Platform::should_close() const {
    return close_window;
}

