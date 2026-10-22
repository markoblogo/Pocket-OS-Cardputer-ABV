#pragma once

#include <M5Unified.hpp>
#include <cstdint>
#include "games_core.h"

// Games owns its state and internal navigation. The firmware owns the canvas,
// theme, keyboard translation, clock and transition back to the Launcher.
namespace games {
struct Palette {
    uint16_t background;
    uint16_t foreground;
    uint16_t dim;
    uint16_t accent;
};

void enter(uint32_t now_ms);
bool portrait();
// Returns true only when Go/Back leaves the Games menu for the Launcher.
bool handleKey(Input key, uint32_t now_ms);
// Returns true when a timed game update needs a redraw.
bool update(uint32_t now_ms);
void draw(LGFX_Sprite& canvas, const Palette& palette);
}  // namespace games
