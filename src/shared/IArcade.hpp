/*
** EPITECH PROJECT, 2026
** tek2
** File description:
** IArcade
*/

#pragma once

#include <string>

namespace arcade {

    // Base interface for all arcade modules (display and game).
    class IArcade {
    public:
        virtual ~IArcade() = default;

        // Returns the human-readable name of the module (e.g. "SDL2", "Snake").
        virtual std::string getName() const = 0;
    };

}
