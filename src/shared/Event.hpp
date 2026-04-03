/*
** EPITECH PROJECT, 2026
** tek2
** File description:
** Event
*/

#pragma once

namespace arcade {

    // All keyboard inputs recognized by the arcade core.
    enum class KeyCode {
        UP, DOWN, LEFT, RIGHT,
        ESCAPE, ENTER, SPACE,
        F1, F2, F3, F4,
        R, M,
        UNKNOWN
    };

    // A single input event produced by a display module each frame.
    struct Event {
        KeyCode key;
    };

} // namespace arcade
