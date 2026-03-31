/*
** EPITECH PROJECT, 2026
** tek2
** File description:
** types
*/

#pragma once

#include <cstdint>

namespace arcade {

    // RGBA color with components in [0, 255]. Defaults to opaque white.
    struct Color {
        uint8_t r = 255;
        uint8_t g = 255;
        uint8_t b = 255;
        uint8_t a = 255;
    };

    // 2D integer coordinate, used for grid positions and pixel positions.
    struct Vec2i {
        int x = 0;
        int y = 0;
    };

    // A single cell in the game grid: a character with foreground/background colors.
    struct Tile {
        char  symbol = ' ';
        Color fg{255, 255, 255, 255};
        Color bg{0,   0,   0,   255};
    };

} // namespace arcade
