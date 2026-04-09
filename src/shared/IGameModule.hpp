/*
** EPITECH PROJECT, 2026
** tek2
** File description:
** IGameModule
*/

#pragma once

#include "IArcade.hpp"
#include "IDisplayModule.hpp"
#include "Event.hpp"
#include <vector>

namespace arcade {

    // Interface for game logic modules. A game module is driven by the core
    // loop: init once, then update/render each tick, stop on exit.
    class IGameModule : public IArcade {
    public:
        ~IGameModule() override = default;

        // Initializes the game state. Called once before the first update.
        virtual void init(IDisplayModule *display)        = 0;
        // Rebinds the display without resetting game state. Called on display switch.
        virtual void setDisplay(IDisplayModule *display)  = 0;
        // Tears down the game state. Called when leaving the game.
        virtual void stop()                               = 0;
        // Advances the game logic by `deltaTime` seconds.
        // `events` contains all non-system input events for this frame.
        // Returns false when the game is over, true to keep running.
        virtual bool update(float deltaTime, const std::vector<Event> &events) = 0;
        // Draws the current game state using the provided display module.
        virtual void render(IDisplayModule *display) = 0;

        // Returns the current player score.
        virtual int getScore() const = 0;
    };

} // namespace arcade
