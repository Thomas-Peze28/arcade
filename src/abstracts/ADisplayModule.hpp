#pragma once

#include "../shared/IDisplayModule.hpp"

namespace arcade {

    class ADisplayModule : public IDisplayModule {
    public:
        ~ADisplayModule() override = default;

        ADisplayModule(const ADisplayModule &) = delete;
        ADisplayModule &operator=(const ADisplayModule &) = delete;

        void open()    override = 0;
        void close()   override = 0;
        void clear()   override = 0;
        void display() override = 0;

        void      setGridSize(Vec2i size) override;
        Vec2i     getGridSize()     const override;

        bool pollEvent(Event &event) override = 0;

        void drawTile(Vec2i pos, const Tile &tile)                     override = 0;
        void drawSprite(Vec2i pos, const std::string &spritePath)      override = 0;
        void drawText(const std::string &text, Vec2i pos, Color color) override = 0;

    protected:
        ADisplayModule() = default;

        bool  _isOpen   = false;
        Vec2i _gridSize = {40, 25};
    };

} // namespace arcade
