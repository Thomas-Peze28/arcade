#pragma once

#include "IArcade.hpp"
#include "Event.hpp"
#include "types.hpp"

namespace arcade {

    class IDisplayModule : public IArcade {
    public:
        ~IDisplayModule() override = default;

        virtual void open()    = 0;
        virtual void close()   = 0;
        virtual void clear()   = 0;
        virtual void display() = 0;

        virtual bool pollEvent(Event &event) = 0;

        virtual void drawTile(Vec2i pos, const Tile &tile)                     = 0;
        virtual void drawText(const std::string &text, Vec2i pos, Color color) = 0;
    };

} // namespace arcade
