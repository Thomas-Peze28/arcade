/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Variables
*/

#ifndef VARIABLES_HPP_
#define VARIABLES_HPP_

#include <string>

namespace Arcade {

    enum class Input {
        NONE,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        Z,
        Q,
        S,
        D,
        SPACE,
        ACTION,
        RESTART,
        MENU,
        EXIT,
        NEXT_GRAPHIC,
        PREV_GRAPHIC,
        NEXT_GAME,
        PREV_GAME
    };

    struct Position {
        int x;
        int y;
    };

    enum class Color {
        RED,
        GREEN,
        BLUE,
        WHITE,
        BLACK
    };

    struct Object {
        Position pos;
        char character;
        Color color;
        std::string sprite_path;
    };
}

#endif /* VARIABLES_HPP_ */