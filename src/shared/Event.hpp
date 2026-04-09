/*
** EPITECH PROJECT, 2026
** tek2
** File description:
** Event
*/

#pragma once

#include "types.hpp"

namespace arcade {

    enum class KeyCode {
        UP, DOWN, LEFT, RIGHT,
        ESCAPE, ENTER, SPACE,
        F1, F2, F3, F4,
        R, M,
        UNKNOWN
    };

    enum class EventType   { KEYBOARD, MOUSE };
    enum class MouseButton { LEFT, RIGHT, MIDDLE };

    struct Event {
        EventType   type        = EventType::KEYBOARD;
        KeyCode     key         = KeyCode::UNKNOWN;
        Vec2i       mousePos    = {};
        MouseButton mouseButton = MouseButton::LEFT;
    };

} // namespace arcade
