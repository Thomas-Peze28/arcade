#pragma once

namespace arcade {

    enum class KeyCode {
        UP, DOWN, LEFT, RIGHT,
        ESCAPE, ENTER, SPACE,
        F1, F2, F3, F4,
        R, M,
        UNKNOWN
    };

    struct Event {
        KeyCode key;
    };

} // namespace arcade
