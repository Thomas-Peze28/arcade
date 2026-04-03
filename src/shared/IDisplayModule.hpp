/*
** EPITECH PROJECT, 2026
** tek2
** File description:
** IDisplayModule
*/

#pragma once

#include "IArcade.hpp"
#include "Event.hpp"
#include "types.hpp"

namespace arcade {

    // Interface for graphical backends. A display module owns a window and
    // provides primitives to draw tiles, sprites, and text each frame.
    class IDisplayModule : public IArcade {
    public:
        ~IDisplayModule() override = default;

        // Creates and shows the window.
        virtual void open()    = 0;
        // Destroys the window and releases its resources.
        virtual void close()   = 0;
        // Erases all content drawn since the last display() call.
        virtual void clear()   = 0;
        // Flushes the current frame to the screen.
        virtual void display() = 0;

        // Sets the playfield size in tile units. Called by the game at init
        // so the renderer can scale its tile size to fit the window.
        virtual void setGridSize(Vec2i size) = 0;
        // Returns the current playfield size in tile units.
        virtual Vec2i getGridSize() const    = 0;

        // Fills `event` with the next pending input event.
        // Returns true if an event was available, false otherwise.
        virtual bool pollEvent(Event &event) = 0;

        // Draws a colored character tile at the given grid position.
        virtual void drawTile(Vec2i pos, const Tile &tile)                     = 0;
        // Draws an image asset (identified by its file path) at the given grid position.
        virtual void drawSprite(Vec2i pos, const std::string &spritePath)      = 0;
        // Draws a string starting at the given grid position with the specified color.
        virtual void drawText(const std::string &text, Vec2i pos, Color color) = 0;
    };

} // namespace arcade
