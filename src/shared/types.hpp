#pragma once

#include <cstdint>

namespace arcade {

    struct Color {
        uint8_t r = 255;
        uint8_t g = 255;
        uint8_t b = 255;
        uint8_t a = 255;
    };

    struct Vec2i {
        int x = 0;
        int y = 0;
    };

    struct Tile {
        char  symbol = ' ';
        Color fg{255, 255, 255, 255};
        Color bg{0,   0,   0,   255};
    };

} // namespace arcade
